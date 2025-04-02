#include "editor.h"

#include "../BVHNode.h"
#include "../Camera.h"
#include "../Hittable.h"
#include "../HittableBuilder.h"
#include "../materials/DiffuseLight.h"
#include "../objects/Box.h"
#include "../objects/Plane.h"
#include "../objects/Sphere.h"
#include "Constants.h"
#include "Utils.h"

#include <ImGuiFileDialog.h>
#include <ImGuizmo.h>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <rlgl.h>

// TODO fix this ugly include
#include <../../vendor/glm/glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <format>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

using namespace rt::constants;

namespace rt {
  bool keyCombination(unsigned int key, std::initializer_list<unsigned int> modifiers) {
    auto const modifiersDown = std::ranges::any_of(modifiers, [](auto mod) { return IsKeyDown(mod); });
    auto const keyDown       = IsKeyPressed(key);
    return modifiersDown && keyDown;
  }

  struct ViewMenuEntry {
    std::string                              name;
    std::function<void(Editor::ViewState &)> onPress;
    std::string                              shortcutStr;
    std::initializer_list<unsigned int>      modifiers;
    unsigned int                             key;
  };

  std::initializer_list<ViewMenuEntry> const viewMenuItems = {
      {"Raytracing settings",
       [](auto &viewState) { viewState.viewMenu.raytracingSettings = !viewState.viewMenu.raytracingSettings; },
       "ALT+R",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_R},
      {"Camera settings",
       [](auto &viewState) { viewState.viewMenu.cameraSettings = !viewState.viewMenu.cameraSettings; },
       "ALT+C",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_C},
      {"Object list",
       [](auto &viewState) { viewState.viewMenu.objectList = !viewState.viewMenu.objectList; },
       "ALT+O",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_O},
      {"Hide all",
       [](auto &viewState) { viewState.viewMenu = {false, false, false}; },
       "ALT+H",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_H},
      {"Show all",
       [](auto &viewState) { viewState.viewMenu = {true, true, true}; },
       "ALT+S",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_S},
  };

  void Editor::Rasterize() {

    BeginTextureMode(rasterRT);
    ClearBackground(getScene()->backgroundColor.toRaylibColor(255));

    BeginMode3D(camera.toRaylibCamera3D());
    {
      DrawGrid(10, 10);

      if (!getScene()->skysphereTexture.empty()) {
        getScene()->skysphere->transformation.setTranslation(camera.getLookFrom());
        getScene()->drawSkysphere();
      }

      auto rasterizables = getScene()->worldRoot->getChildrenAsList();

      for (int i = 0; i < rasterizables.size(); i++) {
        rasterizables[i]->RasterizeTransformed(rasterizables[i]->transformation, vec3(colors[i % numColors]));
      }

      auto aabBs = getScene()->worldRoot->getChildrenAABBs();

      AABB rootAABB;
      getScene()->worldRoot->BoundingBox(0, 1, rootAABB);
      aabBs.push_back(rootAABB);

      for (auto &&bb : aabBs) {
        DrawBoundingBox({bb.min, bb.max}, {255, 0, 255, 255});
      }

      DrawSphere(camera.getLookFrom() + camera.localForward() * camera.focusDist(), 0.05f, LIME);

      DrawLine3D(editor::Camera::lineStart, editor::Camera::lineEnd, BLUE);
    }
    EndMode3D();

    BeginMode2D(Camera2D{Vector2{0}, Vector2{0}, 0.0f, 1.0f});
    camera.DrawFrameOutline();
    EndMode2D();

    EndTextureMode();
  }

  void Editor::RenderImgui() {

    // ImGui::Begin(
    //     "Editor", 0, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);

    // float imguiWidth = ImGui::GetContentRegionAvail().x;
    // float height     = imguiWidth * float(screenRT.texture.height) / screenRT.texture.width;

    // ImGui::End();

    TopMenuImgui();

    RenderViewport();

    if (viewState.viewMenu.objectList) {
      ImGui::Begin("Objects");
      {
        if (dynamic_cast<BVHNode *>(getScene()->worldRoot) != nullptr) {
          if (ImGui::Button("Regenerate BVH", {-1, 0})) {

            // Regenerate tree
            getScene()->worldRoot = getScene()->worldRoot->addChild(nullptr);
          }
        }

        AddObjectImgui();

        ObjectListImgui();
      }
      ImGui::End();
    }

    if (viewState.viewMenu.raytracingSettings)
      RaytraceSettingsImgui();

    if (viewState.viewMenu.cameraSettings)
      camera.RenderImgui();

    SelectedObjectImGui();
  }

  void Editor::FileMenuImGui() {
    constexpr auto *fileMenuTitle = "File";
    if (viewState.fileMenu.shouldOpen) {
      ImGui::OpenPopup(fileMenuTitle);
      viewState.fileMenu.shouldOpen = false;
    }

    if (ImGui::BeginMenu(fileMenuTitle)) {
      if (ImGui::MenuItem("Open scene")) {
        ImGuiFileDialog::Instance()->OpenDialog("OpenScene", "Open scene", ".json",
                                                IGFD::FileDialogConfig{.path = "scenes/"});
      }

      if (ImGui::MenuItem("Save current scene")) {
        ImGuiFileDialog::Instance()->OpenDialog("SaveScene", "SaveScene", ".json",
                                                IGFD::FileDialogConfig{.path = "scenes/"});
      }

      ImGui::Separator();

      if (ImGui::MenuItem("Quit")) {
        app->quit();
      }

      ImGui::EndMenu();
    }

    if (ImGuiFileDialog::Instance()->Display("OpenScene")) {

      if (ImGuiFileDialog::Instance()->IsOk()) {
        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
        app->changeScene(Scene::Load(getScene()->imageWidth, getScene()->imageHeight, filePathName));
      }

      ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("SaveScene")) {

      if (ImGuiFileDialog::Instance()->IsOk()) {

        std::string fileDir  = ImGuiFileDialog::Instance()->GetCurrentPath();
        std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

        std::stringstream sceneNamePlusTime;
        sceneNamePlusTime << fileDir << "/" << fileName;

        json          json = getScene()->toJson();
        std::ofstream outputFile(sceneNamePlusTime.str());
        outputFile << std::setw(4) << json << '\n';
        outputFile.close();
      }

      ImGuiFileDialog::Instance()->Close();
    }
  }

  void Editor::BuiltInMenuImGui() {
    constexpr auto *builtInMenuTitle = "Built in";
    if (viewState.builtInMenu.shouldOpen) {
      ImGui::OpenPopup(builtInMenuTitle);
      viewState.builtInMenu.shouldOpen = false;
    }

    if (ImGui::BeginMenu(builtInMenuTitle)) {
      for (auto &[name, loader] : Scene::builtInScenes) {
        if (ImGui::MenuItem(name.c_str())) {
          app->changeScene(loader(getScene()->imageWidth, getScene()->imageHeight));
        }
      }

      ImGui::EndMenu();
    }
  }

  void Editor::SelectedObjectImGui() {
    if (selectedObject != nullptr) {
      ImGui::Begin("Selected object", 0);

      SelectedObjectGizmo();

      const auto selectedObjectUniqueName = selectedObject->name + "##" + EditorUtils::GetIDFromPointer(selectedObject);
      ImGui::Button(selectedObjectUniqueName.c_str(), {-1, 0});
      selectedObject->OnImgui();

      ImGui::End();
    }
  }

  void Editor::SelectedObjectGizmo() {
    assert(selectedObject != nullptr);

    // ImGuizmo seems to have some issues with ImGui's docking branch. Should
    // apply the patch mentioned in this thread:
    // https://github.com/CedricGuillemet/ImGuizmo/issues/327
    ImGuizmo::BeginFrame();

    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    auto const rotation = selectedObject->transformation.getRotationEuler();

    auto model = [this, rotation] {
      float                 scaleTemp[] = {1.0, 1.0, 1.0};
      std::array<float, 16> model;
      auto const            translation = selectedObject->transformation.getTranslation();
      ImGuizmo::RecomposeMatrixFromComponents(&translation.x, &rotation.x, scaleTemp, model.data());
      return model;
    }();

    auto const viewMatrix = glm::lookAt(camera.getLookFrom().toGlm(),
                                        (camera.getLookFrom() + camera.localForward()).toGlm(), vec3(0, 1, 0).toGlm());

    auto const changed = ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(camera.getProjectionMatrix()),
                                              imguizmoOp, imguizmoMode, model.data());

    if (changed) {
      vec3 translation;
      vec3 newRotation;
      vec3 scale;

      ImGuizmo::DecomposeMatrixToComponents(model.data(), &translation.x, &newRotation.x, &scale.x);

      auto rotationDelta = newRotation - rotation;

      selectedObject->transformation.setTranslation(translation);
      selectedObject->transformation.rotateDelta(rotationDelta);
    }
  }

  void Editor::CheckInput() {
    ImGuiIO &io = ImGui::GetIO();
    if (IsMouseButtonPressed(0) && !io.WantCaptureMouse) {
      float mouseX = GetMouseX();
      float mouseY = GetMouseY();

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

    FileMenuCheckInputs();
    ViewMenuCheckInputs();
    BuiltInMenuCheckInputs();

    if (keyCombination(KEY_Q, {KEY_LEFT_CONTROL, KEY_RIGHT_CONTROL})) {
      app->quit();
    }
  }

  void Editor::FileMenuCheckInputs() {
    if (keyCombination(KEY_F, {KEY_LEFT_ALT, KEY_RIGHT_ALT}))
      viewState.fileMenu.shouldOpen = true;
  }

  void Editor::ViewMenuCheckInputs() {
    if (keyCombination(KEY_V, {KEY_LEFT_ALT, KEY_RIGHT_ALT}))
      viewState.viewMenu.shouldOpen = !viewState.viewMenu.shouldOpen;

    for (auto const &menuItem : viewMenuItems) {
      if (keyCombination(menuItem.key, menuItem.modifiers)) {
        menuItem.onPress(viewState);
      }
    }
  }

  void Editor::BuiltInMenuCheckInputs() {
    if (keyCombination(KEY_B, {KEY_LEFT_ALT, KEY_RIGHT_ALT}))
      viewState.builtInMenu.shouldOpen = true;
  }

  void Editor::ViewMenuImGui() {
    constexpr auto *viewMenuTitle = "View";
    if (viewState.viewMenu.shouldOpen) {
      ImGui::OpenPopup(viewMenuTitle);
      viewState.viewMenu.shouldOpen = false;
    }

    if (ImGui::BeginMenu(viewMenuTitle)) {
      for (auto const &[text, onPress, strShortcut, _, __] : viewMenuItems) {
        if (ImGui::MenuItem(std::format("Toggle {}", text).c_str(), strShortcut.c_str())) {
          onPress(viewState);
        }
      }
      ImGui::EndMenu();
    }
  }

  Hittable *Editor::CastRay(Vector2 mousePos) {

    ::Ray raylibRay = GetMouseRay(mousePos, camera.toRaylibCamera3D());
    Ray   r         = {raylibRay.position, raylibRay.direction, 0};

    editor::Camera::lineStart = r.origin.toGlm();
    editor::Camera::lineEnd   = r.At(1000).toGlm();

    rt::HitRecord rec;
    getScene()->worldRoot->Hit(r, 0, rt::constants::infinity, rec);

    return rec.closestHit;
  }

  void Editor::onUpdate() {

    CheckInput();

    camera.Update(GetFrameTime());

    // Renders into the render texture below ImGui
    Rasterize();

    // Render ImGui on top
    BeginDrawing();
    rlImGuiBegin();

    RenderImgui();

    rlImGuiEnd();
    EndDrawing();
  }

  void Editor::AddObjectImgui() {
    ImGui::Combo(("##" + EditorUtils::GetIDFromPointer(this)).c_str(), (int *)&selectedAddableObject,
                 addableObjectTypes, AddableObjectsTypesCount, -1);
    ImGui::SameLine();
    if (ImGui::Button("+", {-1, 0})) {

      sPtr<Material> defaultMaterial = std::make_shared<DiffuseLight>(vec3(10, 0, 10));
      Hittable      *newRoot;

      switch (selectedAddableObject) {
      case Box: {
        auto added = HittableBuilder<rt::Box>(1)
                         .withMaterial(defaultMaterial)
                         .withName("Added Box##" + EditorUtils::GetIDFromPointer(getScene()->worldRoot));
        newRoot = getScene()->worldRoot->addChild(added.build());
        break;
      }

      case Sphere: {
        auto added = HittableBuilder<rt::Sphere>(1, defaultMaterial)
                         .withMaterial(defaultMaterial)
                         .withName("Added Sphere##" + EditorUtils::GetIDFromPointer(getScene()->worldRoot));
        newRoot = getScene()->worldRoot->addChild(added.build());

        break;
      }

      case Plane: {
        auto added = HittableBuilder<rt::Plane>(1, 1)
                         .withMaterial(defaultMaterial)
                         .withName("Added Plane##" + EditorUtils::GetIDFromPointer(getScene()->worldRoot));
        newRoot = getScene()->worldRoot->addChild(added.build());

        break;
      }

      case AddableObjectsTypesCount:
      default: {
      }
      }

      if (newRoot == nullptr) {
        std::cerr << "Attempting to add a child to a world root that's not a BVHNode or a HittableList\n";
      } else {
        // getScene()->oldWorldRoot = getScene()->worldRoot;
        getScene()->worldRoot = newRoot;
      }
    }
  }

  void Editor::onEnter() {}

  // Regenerate BVH on exit / before entering the raytracer
  void Editor::onExit() {
    auto *scene = getScene();
    auto  rtCam = camera.getRtCamera();

    scene->cam = camera.toSceneCamera();

    scene->imageWidth  = camera.imageWidth();
    scene->imageHeight = camera.imageHeight();

    *app->getARD() = AsyncRenderData(camera.imageWidth(), camera.imageHeight(), app->editorWidth, app->editorHeight,
                                     app->getNumThreads());
  }

  void Editor::RenderViewport() {
    // Flip on Y axis since ImGui and opengl/raylib use different axis
    DrawTextureRec(rasterRT.texture, {0, 0, float(rasterRT.texture.width), -float(rasterRT.texture.height)}, {0, 0},
                   WHITE);
  }

  void Editor::RaytraceSettingsImgui() {
    getScene()->settings.OnImgui();

    ImGui::Begin("Other settings");
    ImGui::ColorEdit3("Background color", &getScene()->backgroundColor.x);
    ImGui::Checkbox("Save on render?", &app->saveOnRender);

    int numThreads = app->getNumThreads();
    if (ImGui::InputScalar("Number of threads", ImGuiDataType_U32, &numThreads)) {
      app->changeNumThreads(numThreads);
    }

    ImGui::End();
  }

  void Editor::ObjectListImgui() {
    auto objects = getScene()->worldRoot->getChildrenAsList();
    for (auto &&o : objects) {
      std::string idPlusName = o->name + "##" + EditorUtils::GetIDFromPointer(o.get());

      // Size of {-1,0} to use full width
      if (ImGui::Button(idPlusName.c_str())) {
        selectedObject = o.get();
      }

      auto const checkboxWidth = ImGui::CalcTextSize("x").x;
      auto const totalWidth    = ImGui::GetContentRegionAvail().x;
      ImGui::SameLine(totalWidth - checkboxWidth);
      ImGui::SetNextItemWidth(totalWidth - checkboxWidth);

      // Remove current object from world and rebuild
      if (ImGui::Button(("x##" + EditorUtils::GetIDFromPointer(o.get())).c_str())) {
        getScene()->worldRoot = getScene()->worldRoot->removeChild(o);
      }

      ImGui::Separator();
    }
  }

  std::optional<sPtr<Material>> Editor::MaterialChanger() {

    static const char   *materialTypes[]      = {"Diffuse", "Dielectric", "Metal", "Emissive"};
    static MaterialTypes selectedMaterialType = Emissive;

    ImGui::Combo(("##" + EditorUtils::GetIDFromPointer(materialTypes)).c_str(), (int *)&selectedMaterialType,
                 materialTypes, MaterialTypes::MaterialTypesCount);

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

    auto imageWidth  = getScene()->imageWidth;
    auto imageHeight = getScene()->imageHeight;

    if (ImGui::BeginMainMenuBar()) {

      FileMenuImGui();
      ViewMenuImGui();
      BuiltInMenuImGui();

      ImGui::EndMainMenuBar();
    }
  }

  void Editor::changeScene(Scene *scene) { camera.updateFromRtCamera(scene->cam); }
} // namespace rt
