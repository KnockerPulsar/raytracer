#include "editor.h"

// Whines about imgui.h
// It's included in premake's includedirs
// Things build fine
#include "../../vendor/ImGuiFileDialog/ImGuiFileDialog.h"

#include "../../vendor/glm/glm/glm.hpp"
#include "../../vendor/glm/glm/gtc/type_ptr.hpp"
#include "../../vendor/glm/glm/gtx/transform.hpp"
#include "../../vendor/imguizmo/ImGuizmo.h"
#include "../../vendor/rlImGui/rlImGui.h"
#include "../BVHNode.h"
#include "../Camera.h"
#include "../Hittable.h"
#include "../HittableList.h"
#include "../materials/DiffuseLight.h"
#include "../objects/Box.h"
#include "../objects/Plane.h"
#include "../objects/Sphere.h"
#include "Utils.h"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <raylib.h>
#include <rlgl.h>
#include <sstream>
#include <string>
#include <vector>

#include "../HittableBuilder.h"
#include "imgui.h"

namespace rt {

  void Editor::Rasterize() {

    BeginTextureMode(screenRT);
    ClearBackground(currentScene->backgroundColor.toRaylibColor(255));

    BeginMode3D(cam->toRaylibCamera3D());
    {
      DrawGrid(10, 10);

      if (!currentScene->skysphereTexture.empty()) {
        currentScene->skysphere->transformation.translate = cam->lookFrom;
        currentScene->drawSkysphere();
      }

      auto rasterizables = currentScene->worldRoot->getChildrenAsList();

      for (int i = 0; i < rasterizables.size(); i++) {
        rasterizables[i]->RasterizeTransformed(rasterizables[i]->transformation, vec3(colors[i % numColors]));
      }

      auto aabBs = currentScene->worldRoot->getChildrenAABBs();

      AABB rootAABB;
      currentScene->worldRoot->BoundingBox(0, 1, rootAABB);
      aabBs.push_back(rootAABB);

      for (auto &&bb : aabBs) {
        DrawBoundingBox({bb.min, bb.max}, {255, 0, 255, 255});
      }

      DrawSphere(cam->lookFrom + cam->localForward * cam->focusDist, 0.05f, LIME);

      DrawLine3D(rt::Camera::lineStart, rt::Camera::lineEnd, BLUE);
    }
    EndMode3D();
    EndTextureMode();
  }

  void Editor::RenderImgui() {

    // ImGui::Begin(
    //     "Editor", 0, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);

    // float imguiWidth = ImGui::GetContentRegionAvail().x;
    // float height     = imguiWidth * float(screenRT.texture.height) / screenRT.texture.width;

    // ImGui::End();

    TopMenuImgui();

    // TODO: Use correct parameters when viewport is figured out
    // Currently parameters are not used inside the function.
    UpdateViewportRect(-1, -1);

    RenderViewport();

    ImGui::Begin("Objects");
    {
      if (dynamic_cast<BVHNode *>(currentScene->worldRoot) != nullptr) {
        if (ImGui::Button("Regenerate BVH", {-1, 0})) {

          // Regenerate tree
          currentScene->worldRoot = currentScene->worldRoot->addChild(nullptr);
        }
      }

      AddObjectImgui();

      ObjectListImgui();
    }
    ImGui::End();

    RaytraceSettingsImgui();

    SelectedObjectGizmo();
  }

  void Editor::SelectedObjectGizmo() const {
    if (selectedObject != nullptr) {

      const auto selectedObjectUniqueName = selectedObject->name + "##" + EditorUtils::GetIDFromPointer(selectedObject);

      float scaleTemp[3];
      float model[16];
      ImGuizmo::RecomposeMatrixFromComponents(
          &selectedObject->transformation.translate.x, &selectedObject->transformation.rotate.x, scaleTemp, model
      );
      ImGuizmo::BeginFrame();

      ImGui::Begin("Selected object", 0, ImGuiWindowFlags_AlwaysAutoResize);

      // Tell ImGuizmo to draw in the foreground
      // ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

      // Set viewport for ImGuizmo for proper Gizmo placement
      ImGuizmo::SetRect(viewportMin.x, viewportMin.y, viewportSize.x, viewportSize.y);

      // Tell ImGui to not draw outside the viewport
      ImGui::PushClipRect(viewportMin, viewportMax, false);

      // clang-format off
        ImGuizmo::Manipulate(
          glm::value_ptr(cam->getViewMatrix()),
    glm::value_ptr(cam->getProjectionMatrix()),
     imguizmoOp,
          imguizmoMode,
        model
        );
      // clang-format on
      if (ImGuizmo::IsUsing()) {

        vec3 translation;
        vec3 rotation;
        vec3 scale;

        ImGuizmo::DecomposeMatrixToComponents(model, &translation.x, &rotation.x, &scale.x);

        selectedObject->transformation.translate = translation;
        selectedObject->transformation.rotate    = rotation;
      }

      ImGui::Button(selectedObjectUniqueName.c_str(), {-1, 0});
      selectedObject->OnImgui();

      ImGui::End();
    }
  }

  void Editor::CheckInput() {
    ImGuiIO &io = ImGui::GetIO();
    // std::cout << io.WantCaptureMouse << std::endl;
    if (IsMouseButtonPressed(0) && !io.WantCaptureMouse
        /*&& mouseInsideEditorViewport*/) {
      // Trial-and-error'd my way through figuring out these

      float mouseX = GetMouseX() - viewportMin.x;
      float mouseY = GetMouseY() - viewportMin.y;

      float mouseU = mouseX / (viewportMax.x - viewportMin.x);
      float mouseV = mouseY / (viewportMax.y - viewportMin.y);

      mouseX = mouseU * (GetScreenWidth());
      mouseY = mouseV * (GetScreenHeight());

      // std::cout << mouseU << "," << mouseV << "\n";

      selectedObject = CastRay({mouseX, mouseY});
    }

    if (!io.WantCaptureKeyboard) {

      if (IsKeyDown(KEY_R)) {
        imguizmoOp = ImGuizmo::OPERATION::ROTATE;
      }
      if (IsKeyDown(KEY_T)) {
        imguizmoOp = ImGuizmo::OPERATION::TRANSLATE;
      }
      if (IsKeyDown(KEY_Y)) {
        imguizmoOp = ImGuizmo::OPERATION::SCALE;
      }
      if (IsKeyPressed(KEY_X)) {
        imguizmoMode = imguizmoMode == ImGuizmo::MODE::LOCAL ? ImGuizmo::MODE::WORLD : ImGuizmo::MODE::LOCAL;
      }
    }
  }

  Hittable *Editor::CastRay(Vector2 mousePos) const {

    ::Ray raylibRay = GetMouseRay(mousePos, cam->toRaylibCamera3D());
    Ray   r         = {raylibRay.position, raylibRay.direction, 0};

    rt::Camera::lineStart = r.origin.toGlm();
    rt::Camera::lineEnd   = r.At(1000).toGlm();

    rt::HitRecord rec;
    currentScene->worldRoot->Hit(r, 0, infinity, rec);

    return rec.closestHit;
  }

  void Editor::onUpdate() {

    CheckInput();

    cam->Update(GetFrameTime());
    Rasterize();

    BeginDrawing();
    rlImGuiBegin();

    RenderImgui();
    cam->RenderImgui();

    rlImGuiEnd();
    EndDrawing();
  }

  void Editor::AddObjectImgui() {
    ImGui::Combo(
        ("##" + EditorUtils::GetIDFromPointer(this)).c_str(),
        (int *)&selectedAddableObject,
        addableObjectTypes,
        AddableObjectsTypesCount,
        -1
    );
    ImGui::SameLine();
    if (ImGui::Button("+", {-1, 0})) {

      sPtr<Material> defaultMaterial = std::make_shared<DiffuseLight>(vec3(10, 0, 10));
      Hittable      *newRoot;

      switch (selectedAddableObject) {
      case Box: {
        auto added = HittableBuilder<rt::Box>(1)
                         .withMaterial(defaultMaterial)
                         .withName("Added Box##" + EditorUtils::GetIDFromPointer(currentScene->worldRoot));
        newRoot = currentScene->worldRoot->addChild(added.build());
        break;
      }

      case Sphere: {
        auto added = HittableBuilder<rt::Sphere>(1, defaultMaterial)
                         .withMaterial(defaultMaterial)
                         .withName("Added Sphere##" + EditorUtils::GetIDFromPointer(currentScene->worldRoot));
        newRoot = currentScene->worldRoot->addChild(added.build());

        break;
      }

      case Plane: {
        auto added = HittableBuilder<rt::Plane>(1, 1)
                         .withMaterial(defaultMaterial)
                         .withName("Added Plane##" + EditorUtils::GetIDFromPointer(currentScene->worldRoot));
        newRoot = currentScene->worldRoot->addChild(added.build());

        break;
      }

      case AddableObjectsTypesCount:
      default: {
      }
      }

      if (newRoot == nullptr) {
        std::cerr << "Attempting to add a child to a world root that's not a BVHNode or a HittableList\n";
      } else {
        // currentScene->oldWorldRoot = currentScene->worldRoot;
        currentScene->worldRoot = newRoot;
      }
    }
  }

  // Regenerate BVH on exit / before entering the raytracer
  void Editor::onExit() {}

  void Editor::UpdateViewportRect(float imguiWidth, float height) {

    viewportMin = {0, 0};
    viewportMax = ImVec2(GetScreenWidth(), GetScreenHeight());

    // TODO: Uncomment this for proper viewport raycasting when you figure viewports out.
    // viewportMin = {0, 0};
    // viewportMax = {imguiWidth, height};
    // viewportMin.x += ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x;
    // viewportMin.y += ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y;
    // viewportMax.x += ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x;
    // viewportMax.y += ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y;

    viewportSize = {viewportMax.x - viewportMin.x, viewportMax.y - viewportMin.y};

    mouseInsideEditorViewport = ImGui::IsMouseHoveringRect(viewportMin, viewportMax);
  }

  void Editor::RenderViewport() {
    // TODO: Figure out a way to render to an ImGui image AND register gizmo hits
    // ImGui::Image(&screenRT.texture.id, {float(viewportSize.x), float(viewportSize.y)}, {0, 1}, {1, 0});
    // ImGui::GetForegroundDrawList()->AddRect(viewportMin, viewportMax, IM_COL32(255, 255, 0, 255));

    // Flip on Y axis since ImGui and opengl/raylib use different axis
    DrawTextureRec(
        screenRT.texture, {0, 0, float(screenRT.texture.width), -float(screenRT.texture.height)}, {0, 0}, WHITE
    );
  }

  void Editor::RaytraceSettingsImgui() {
    currentScene->settings.OnImgui();
    ImGui::ColorEdit3("Background color", &currentScene->backgroundColor.x);
  }

  void Editor::ObjectListImgui() {
    auto objects = currentScene->worldRoot->getChildrenAsList();
    for (auto &&o : objects) {
      std::string idPlusName = o->name + "##" + EditorUtils::GetIDFromPointer(o.get());

      // Size of {-1,0} to use full width
      if (ImGui::Button(idPlusName.c_str())) {
        selectedObject = o.get();
      }

      ImGui::SameLine();

      // Remove current object from world and rebuild
      if (ImGui::Button(("x##" + EditorUtils::GetIDFromPointer(o.get())).c_str())) {
        currentScene->worldRoot = currentScene->worldRoot->removeChild(o);
      }

      ImGui::Separator();
    }
  }

  std::optional<sPtr<Material>> Editor::MaterialChanger() {

    static const char   *materialTypes[]      = {"Diffuse", "Dielectric", "Metal", "Emissive"};
    static MaterialTypes selectedMaterialType = Emissive;

    ImGui::Combo(
        ("##" + EditorUtils::GetIDFromPointer(materialTypes)).c_str(),
        (int *)&selectedMaterialType,
        materialTypes,
        MaterialTypes::MaterialTypesCount
    );

    ImGui::SameLine();
    if (ImGui::Button("Change")) {

      switch (selectedMaterialType) {

      case MaterialTypes::Emissive: {
        return std::make_optional(std::make_shared<DiffuseLight>(vec3(1, 1, 1)));
        break;
      }

      case MaterialTypes::Diffuse: {
        return std::make_optional(std::make_shared<Lambertian>(vec3(0.4, 0.6, 0.8)));
        break;
      }

      case MaterialTypes::Dielectrical: {
        return std::make_optional(std::make_shared<Dielectric>(1.3, vec3(0.9, 0.9, 0.9)));
        break;
      }

      case MaterialTypes::Metallic: {
        return std::make_optional(std::make_shared<Metal>(vec3(0.3, 0.3, 0.3), 0.7));
      }

      default:
        return std::nullopt;
      }
    }
    return std::nullopt;
  }

  void Editor::TopMenuImgui() {

    auto imageWidth  = currentScene->imageWidth;
    auto imageHeight = currentScene->imageHeight;

    if (ImGui::BeginMainMenuBar()) {

      if (ImGui::BeginMenu("Scene")) {
        if (ImGui::MenuItem("Open scene")) {
          ImGuiFileDialog::Instance()->OpenDialog("OpenScene", "Open scene", ".json", "scenes/");
        }

        if (ImGui::MenuItem("Save current scene")) {
          ImGuiFileDialog::Instance()->OpenDialog("SaveScene", "SaveScene", ".json", "scenes/");
        }

        ImGui::EndMenu();
      }

      std::vector<std::pair<std::string, std::function<Scene(int, int)>>> builtInScenes = {
          {"Default", Scene::Default},
          {"Scene1", Scene::Scene1},
          {"Scene2", Scene::Scene2},
          {"Random", std::bind(Scene::Random, std::placeholders::_1, std::placeholders::_2, 11, 11)},
          {"Random Moving",
           std::bind(Scene::RandomMovingSpheres, std::placeholders::_1, std::placeholders::_2, 11, 11)},
          {"TwoSpheres", Scene::TwoSpheres},
          {"Earth", Scene::Earth},
          {"Light", Scene::Light},
          {"Cornell", Scene::CornellBox},
          {"Transformation test", Scene::TransformationTest},
          {"Plane test", Scene::PlaneTest},
          {"Raster test", Scene::RasterTest}};

      if (ImGui::BeginMenu("Built in")) {
        for (auto &[name, loader] : builtInScenes) {
          if (ImGui::MenuItem(name.c_str())) {
            *currentScene = loader(imageWidth, imageHeight);
          }
        }

        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    // display
    if (ImGuiFileDialog::Instance()->Display("OpenScene")) {

      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk()) {
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        // std::string filePath     = ImGuiFileDialog::Instance()->GetCurrentPath();

        // action
        *currentScene = Scene::Load(imageWidth, imageHeight, filePathName);
      }

      // close
      ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("SaveScene")) {

      // action if OK
      if (ImGuiFileDialog::Instance()->IsOk()) {

        std::string fileDir  = ImGuiFileDialog::Instance()->GetCurrentPath();
        std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

        // action

        std::stringstream sceneNamePlusTime;
        sceneNamePlusTime << fileDir << "/" << fileName;

        json          json = currentScene->toJson();
        std::ofstream outputFile(sceneNamePlusTime.str());
        outputFile << std::setw(4) << json << std::endl;
        outputFile.close();
      }

      // close
      ImGuiFileDialog::Instance()->Close();
    }
  }
} // namespace rt