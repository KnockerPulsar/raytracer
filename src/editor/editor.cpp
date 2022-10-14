#include "editor.h"

// Whines about imgui.h
// It's included in premake's includedirs
// Things build fine
#include "ImGuiFileDialog.h"

#include "../../vendor/glm/glm/glm.hpp"
#include "../../vendor/glm/glm/gtc/type_ptr.hpp"
#include "../../vendor/glm/glm/gtx/transform.hpp"
#include "../../vendor/imguizmo/ImGuizmo.h"
#include "../../vendor/rlImGui/rlImGui.h"

#include "BVHNode.h"
#include "Camera.h"
#include "Hittable.h"
#include "HittableList.h"

#include "materials/DiffuseLight.h"
#include "materials/MaterialBuilder.h"

#include "objects/Box.h"
#include "objects/Plane.h"
#include "objects/Sphere.h"

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
#include "textures/CheckerTexture.h"
#include "textures/Texture.h"

namespace rt {

  void Editor::Rasterize() {

    BeginTextureMode(App::getARD()->rasterRT);
    ClearBackground(App::scene.backgroundColor.toRaylibColor(255));

    BeginMode3D(getCamera()->toRaylibCamera3D());
    {
      DrawGrid(10, 10);

      if (!App::scene.skysphereTexture.empty()) {
        App::scene.skysphere->transformation.setTranslation(getCamera()->lookFrom);
        App::scene.drawSkysphere();
      }

      auto rasterizables = App::scene.worldRoot->getChildrenAsList();

      for (int i = 0; i < rasterizables.size(); i++) {
        rasterizables[i]->RasterizeTransformed(rasterizables[i]->transformation, vec3(colors[i % numColors]));
      }

      auto aabBs = App::scene.worldRoot->getChildrenAABBs();

      AABB rootAABB;
      App::scene.worldRoot->BoundingBox(0, 1, rootAABB);
      aabBs.push_back(rootAABB);

      for (auto &&bb : aabBs) {
        DrawBoundingBox({bb.min.toRlVec3(), bb.max.toRlVec3()}, {255, 0, 255, 255});
      }

      vec3 focusSpherePos = getCamera()->lookFrom + getCamera()->localForward * getCamera()->focusDist;
      DrawSphere(focusSpherePos.toRlVec3(), 0.05f, LIME);

      DrawLine3D(rt::Camera::lineStart.toRlVec3(), rt::Camera::lineEnd.toRlVec3(), BLUE);
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
      if (dynamic_cast<BVHNode *>(App::scene.worldRoot) != nullptr) {
        if (ImGui::Button("Regenerate BVH", {-1, 0})) {

          // Regenerate tree
          App::scene.worldRoot = App::scene.worldRoot->addChild(nullptr);
        }
      }

      AddObjectImgui();

      ObjectListImgui();
    }
    ImGui::End();

    RaytraceSettingsImgui();

    SelectedObjectGizmo();
  }

  void Editor::SelectedObjectGizmo() {
    if (selectedObject != nullptr) {

      const auto selectedObjectUniqueName = selectedObject->name + "##" + EditorUtils::GetIDFromPointer(selectedObject);

      float scaleTemp[3];
      float model[16];
      ImGuizmo::RecomposeMatrixFromComponents(
          &selectedObject->transformation.getTranslation().x, &selectedObject->transformation.getRotation().x, scaleTemp, model
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
          glm::value_ptr(getCamera()->getViewMatrix()),
    glm::value_ptr(getCamera()->getProjectionMatrix()),
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

        selectedObject->transformation
          .setTranslation(translation)
          .setRotation(rotation);
      }

      ImGui::Button(selectedObjectUniqueName.c_str(), {-1, 0});
      selectedObject->OnBaseImgui();

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

  Hittable *Editor::CastRay(Vector2 mousePos) {

    ::Ray raylibRay = GetMouseRay(mousePos, getCamera()->toRaylibCamera3D());
    Ray   r         = {raylibRay.position, raylibRay.direction, 0};

    rt::Camera::lineStart = r.origin;
    rt::Camera::lineEnd   = r.At(1000);

    rt::HitRecord rec;
    App::scene.worldRoot->Hit(r, 0, infinity, rec);

    return rec.closestHit;
  }

  void Editor::onUpdate() {

    CheckInput();

    getCamera()->Update(GetFrameTime());
    Rasterize();

    BeginDrawing();
    rlImGuiBegin();

    RenderImgui();
    getCamera()->RenderImgui();

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

      sPtr<Material> defaultMaterial = MaterialBuilder<DiffuseLight>().setTexture(vec3(10, 0, 10)).build();
      Hittable      *newRoot;

      switch (selectedAddableObject) {
      case Box: {
        auto added = HittableBuilder<rt::Box>(1)
                         .withMaterial(defaultMaterial)
                         .withName("Added Box##" + EditorUtils::GetIDFromPointer(App::scene.worldRoot));
        newRoot = App::scene.worldRoot->addChild(added.build());
        break;
      }

      case Sphere: {
        auto added = HittableBuilder<rt::Sphere>(1, defaultMaterial)
                         .withMaterial(defaultMaterial)
                         .withName("Added Sphere##" + EditorUtils::GetIDFromPointer(App::scene.worldRoot));
        newRoot = App::scene.worldRoot->addChild(added.build());

        break;
      }

      case Plane: {
        auto added = HittableBuilder<rt::Plane>(1, 1)
                         .withMaterial(defaultMaterial)
                         .withName("Added Plane##" + EditorUtils::GetIDFromPointer(App::scene.worldRoot));
        newRoot = App::scene.worldRoot->addChild(added.build());

        break;
      }

      case AddableObjectsTypesCount:
      default: {
      }
      }

      if (newRoot == nullptr) {
        std::cerr << "Attempting to add a child to a world root that's not a BVHNode or a HittableList\n";
      } else {
        // App::scene.oldWorldRoot = App::scene.worldRoot;
        App::scene.worldRoot = newRoot;
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
        App::getARD()->rasterRT.texture,
        {0, 0, float(App::getEditorWidth()), -float(App::getEditorHeight())},
        {0, 0},
        WHITE
    );
  }

  void Editor::RaytraceSettingsImgui() {

    ImGui::Begin("Raytrace settings");
    ImGui::DragInt("Samples per pixel", &App::rtSettings.samplesPerPixel, 1, 1, 500);
    ImGui::DragInt("Maximum depth", &App::rtSettings.maxDepth, 1, 1, 100);
    ImGui::End();
    
    ImGui::Begin("Other settings");
    ImGui::ColorEdit3("Background color", &App::scene.backgroundColor.x);
    ImGui::Checkbox("Save on render?", &App::saveOnRender);

    int numThreads = App::getNumThreads();
    if(ImGui::InputScalar("Number of threads", ImGuiDataType_U32, &numThreads)) {
      App::changeNumThreads(numThreads);
    }
    
    ImGui::End();
  }

  void Editor::ObjectListImgui() {
    auto objects = App::scene.worldRoot->getChildrenAsList();
    for (auto &&o : objects) {
      std::string idPlusName = o->name + "##" + EditorUtils::GetIDFromPointer(o.get());

      // Size of {-1,0} to use full width
      if (ImGui::Button(idPlusName.c_str())) {
        selectedObject = o.get();
      }

      ImGui::SameLine();

      // Remove current object from world and rebuild
      if (ImGui::Button(("x##" + EditorUtils::GetIDFromPointer(o.get())).c_str())) {
        App::scene.worldRoot = App::scene.worldRoot->removeChild(o);
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
    if (ImGui::Button("Change Material")) {

      switch (selectedMaterialType) {

      case MaterialTypes::Emissive: {
        return std::make_optional(MaterialBuilder<DiffuseLight>().setTexture(vec3(1, 1, 1)).build());
      }

      case MaterialTypes::Diffuse: {
        return std::make_optional(MaterialBuilder<Lambertian>().setTexture(vec3(0.4, 0.6, 0.8)).build());
      }

      case MaterialTypes::Dielectrical: {
        return std::make_optional(MaterialBuilder<Dielectric>(1.3).setTexture(vec3(0.9, 0.9, 0.9)).build());
      }

      case MaterialTypes::Metallic: {
        return std::make_optional(MaterialBuilder<Metal>(0.7f).setTexture(vec3(0.3, 0.3, 0.3)).build());
      }

      default:
        return std::nullopt;
      }
    }
    return std::nullopt;
  }

  std::optional<sPtr<Texture>> Editor::TextureChanger() {
    static const char   *textureTypes[]      = { "SolidColor", "ImageTexture", "Noise", "Checker "};
    static TextureTypes selectedTextureType  = TextureTypes::Checker ;

    ImGui::Combo(
        ("##" + EditorUtils::GetIDFromPointer(textureTypes)).c_str(),
        (int *)&selectedTextureType,
        textureTypes,
        TextureTypes::TextureTypesCount
    );

    ImGui::SameLine();
    if (ImGui::Button("Change Texture")) {
    
      switch (selectedTextureType) {
      case TextureTypes::SolidColorTex: {
       return std::make_optional(std::make_shared<rt::SolidColor>(vec3(1,0,1))) ;
      } 
      
      case TextureTypes::Checker: {
        return std::make_optional(std::make_shared<rt::CheckerTexture>(vec3(0,0,0), vec3(1,1,1)));
      }

      case TextureTypes::ImageTex: {
        return std::make_optional(std::make_shared<rt::ImageTexture>());
      }
      
      case TextureTypes::Noise: {
        return std::make_optional(std::make_shared<rt::NoiseTexture>());
      }

      default:
        return std::nullopt;
      }
    }
    return std::nullopt;
  }

  void Editor::TopMenuImgui() {

    auto imageWidth  = App::getImageWidth();
    auto imageHeight = App::getImageHeight();

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

      if (ImGui::BeginMenu("Built in")) {
        for (auto &[name, loader] : Scene::builtInScenes) {
          if (ImGui::MenuItem(name.c_str())) {
            App::changeScene(loader());
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
        App::changeScene(Scene::Load(filePathName));
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

        json          json = App::scene.toJson();
        std::ofstream outputFile(sceneNamePlusTime.str());
        outputFile << std::setw(4) << json << std::endl;
        outputFile.close();
      }

      // close
      ImGuiFileDialog::Instance()->Close();
    }
  }
} // namespace rt
