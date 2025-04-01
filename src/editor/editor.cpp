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
#include "Util.h"
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

namespace rt {
    struct ViewMenuEntry {
      std::string name;
      std::function<void(Editor::ViewState&)> onPress;
      std::string shortcutStr;
      std::initializer_list<unsigned int> modifiers;
      unsigned int key;
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

        DrawLine3D(Editor::Camera::lineStart, Editor::Camera::lineEnd, BLUE);
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

    if(viewState.viewMenu.raytracingSettings)
      RaytraceSettingsImgui();

    if(viewState.viewMenu.cameraSettings)
      camera.RenderImgui();

    SelectedObjectImGui();
  }

  void Editor::FileMenuImGui()
  {
      constexpr auto* fileMenuTitle = "File";
      if(viewState.fileMenu.shouldOpen)
      {
        ImGui::OpenPopup(fileMenuTitle);
        viewState.fileMenu.shouldOpen = false;
      }

      if (ImGui::BeginMenu(fileMenuTitle)) {
        if (ImGui::MenuItem("Open scene")) {
          ImGuiFileDialog::Instance()->OpenDialog(
              "OpenScene", "Open scene", ".json", IGFD::FileDialogConfig{.path = "scenes/"}
          );
        }

        if (ImGui::MenuItem("Save current scene")) {
          ImGuiFileDialog::Instance()->OpenDialog(
              "SaveScene", "SaveScene", ".json", IGFD::FileDialogConfig{.path = "scenes/"}
          );
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

  void Editor::BuiltInMenuImGui()
  {
      constexpr auto* builtInMenuTitle = "Built in";
      if(viewState.builtInMenu.shouldOpen)
      {
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

  void Editor::SelectedObjectImGui()
  {
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

    auto const            rotation    = selectedObject->transformation.getRotationEuler();

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

    {
      auto const ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
      auto const qDown    = IsKeyPressed(KEY_Q);
      if (ctrlDown && qDown)
        app->quit();
    }
  }

  void Editor::FileMenuCheckInputs() {
    auto const altDown = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    auto const fDown   = IsKeyPressed(KEY_F);

    if (altDown && fDown)
      viewState.fileMenu.shouldOpen = true;
  }

  void Editor::ViewMenuCheckInputs() {
    {
      auto const altDown = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
      auto const vDown = IsKeyPressed(KEY_V);

      if(altDown && vDown)
        viewState.viewMenu.shouldOpen = !viewState.viewMenu.shouldOpen;
    }

    for (auto const &menuItem : viewMenuItems) {
      auto const anyModifierDown =
          std::ranges::any_of(menuItem.modifiers, [](auto modifier) { return IsKeyDown(modifier); });
      auto const buttonPressed = IsKeyPressed(menuItem.key);

      if (anyModifierDown && buttonPressed) {
        menuItem.onPress(viewState);
      }
    }
  }

  void Editor::BuiltInMenuCheckInputs() {
    auto const altDown = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    auto const bDown   = IsKeyPressed(KEY_B);

    if (altDown && bDown)
      viewState.builtInMenu.shouldOpen = true;
  }

  void Editor::ViewMenuImGui()
  {
      constexpr auto *viewMenuTitle = "View";
      if (viewState.viewMenu.shouldOpen)
      {
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

    Editor::Camera::lineStart = r.origin.toGlm();
    Editor::Camera::lineEnd   = r.At(1000).toGlm();

    rt::HitRecord rec;
    getScene()->worldRoot->Hit(r, 0, infinity, rec);

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
    auto rtCam = camera.getRtCamera();

    scene->cam = camera.toSceneCamera();

    scene->imageWidth = camera.imageWidth();
    scene->imageHeight = camera.imageHeight();

    *app->getARD() = AsyncRenderData(camera.imageWidth(), camera.imageHeight(),
                                     app->editorWidth, app->editorHeight,
                                     app->getNumThreads());
  }

  void Editor::RenderViewport() {
    // Flip on Y axis since ImGui and opengl/raylib use different axis
    DrawTextureRec(
        rasterRT.texture,
        {0, 0, float(rasterRT.texture.width), -float(rasterRT.texture.height)},
        {0, 0},
        WHITE
    );
  }

  void Editor::RaytraceSettingsImgui() {
    getScene()->settings.OnImgui();

    ImGui::Begin("Other settings");
    ImGui::ColorEdit3("Background color", &getScene()->backgroundColor.x);
    ImGui::Checkbox("Save on render?", &app->saveOnRender);

    int numThreads = app->getNumThreads();
    if(ImGui::InputScalar("Number of threads", ImGuiDataType_U32, &numThreads)) {
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

      ImGui::SameLine();

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

    auto imageWidth  = getScene()->imageWidth;
    auto imageHeight = getScene()->imageHeight;

    if (ImGui::BeginMainMenuBar()) {

      FileMenuImGui();
      ViewMenuImGui();
      BuiltInMenuImGui();

      ImGui::EndMainMenuBar();
    }
  }

  void Editor::changeScene(Scene *scene) {
    camera.updateFromRtCamera(scene->cam);
  }

  void Editor::Camera::DrawFrameOutline() const {
    if(!frameSizeAffectsCrop)
      return;

    auto constexpr thickness = 5.0;
    // Account for titlebar
    auto constexpr titlebarClearence = 4;
    auto rectX     = (editorWidth - _imageWidth) / 2;
    auto rectY     = (editorHeight - _imageHeight) / 2;
    DrawRectangleLinesEx(::Rectangle(rectX, rectY + (thickness * titlebarClearence), _imageWidth - thickness,
                                     _imageHeight - (thickness * titlebarClearence)),
                         thickness, MAGENTA);
  }

  void Editor::Camera::RenderImgui() {
    if (ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
      if(ImGui::DragFloat3("lookFrom", &rtCamera.lookFrom.x, 0.05))
      {
        rtCamera.lookAt = rtCamera.lookFrom + rtCamera.localForward;
      }

      ImGui::Combo("Camera type", (int *)&controlType, controlTypeLabels,
                   static_cast<int>(Editor::Camera::ControlType::controlTypesCount), 0);

      if (controlType == ControlType::lookAtPoint) {
        ImGui::DragFloat3("rtCamera.lookAt", &rtCamera.lookAt.x, 0.05);
      } else {
        if(ImGui::DragFloat2("rotation", &angle.x, 0.05f))
          lookAtAngle(angle);
      }

      ImGui::DragFloat("Vertical FOV", &rtCamera.vFov, 1.0, 5.0, 179.0f);

      {
        ImGui::Checkbox("Frame size affects crop?", &frameSizeAffectsCrop);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
          ImGui::SetTooltip(
              "Whether setting a smaller/bigger frame size crops/expands the rendered image or just renders what you "
              "see at a lower/higher resolution.");
        }
      }

      ImGui::Separator();

      ImGui::DragFloat("Camera speed multiplier", &movementSpeedMultiplier, 0.1, 1, 100);

      ImGui::DragFloat("Camera boost multiplier", &boostMultiplier, 0.1, 1, 10);

      ImGui::DragInt2("frame size", &_imageWidth);
    }

    ImGui::End();
  }

  Editor::Camera::Camera(int editorWidth, int editorHeight, int imageWidth, int imageHeight, Scene const &initialScene)
      : editorWidth(editorWidth), editorHeight(editorHeight), _imageWidth(imageWidth), _imageHeight(imageHeight),
        rtCamera(initialScene.cam), controlType(ControlType::flyCam) {
    updateFromRtCamera(rtCamera);
  }

  void Editor::Camera::updateFromRtCamera(rt::Camera const &sceneCamera) {
    rtCamera = sceneCamera;
    rtCamera.RecomputeLocalBases();

    forwardToAngle(localForward());
    lookAtAngle(angle);
  }

  void Editor::Camera::UpdateRtCamera() {
    rtCamera.aspectRatio = static_cast<float>(_imageWidth) / _imageHeight;
    rtCamera.RecomputeLocalBases();
  }

  void Editor::Camera::Fwd(float deltaTime) {
    rtCamera.lookFrom += moveDir * deltaTime;
    UpdateRtCamera();
  }

  void Editor::Camera::Bck(float deltaTime) {
    rtCamera.lookFrom += -moveDir * deltaTime;
    UpdateRtCamera();
  }

  void Editor::Camera::lookAtAngle(vec3 angle) {
    // x = r * cos(90 - angle.y) * sin(90 - angle.x)
    // y = r * cos(90 - angle.x)
    // z = r * sin(90 - angle.y) * sin(90 - angle.x)
    auto const dir = vec3{
        cos(pi / 2 - angle.y),
        cos(angle.x - pi / 2),
        sin(pi / 2 - angle.y),
    };

    rtCamera.lookAt = rtCamera.lookFrom + dir;
    UpdateRtCamera();
  }

  void Editor::Camera::forwardToAngle(vec3 fwd) {
    assert(fwd.SqrLen() > 0);

    angle.x = pi / 2 + std::acos(fwd.y);
    angle.y = std::atan2(fwd.x, fwd.z);

    if (angle.x >= pi) {
      angle.x = pi - angle.x;
    }
    angle.y = glm::wrapAngle(angle.y);
  }

  void Editor::Camera::MouseLook(Vector2 mousePositionDelta) {
    angle.x += (mousePositionDelta.y * -rotSensitity.y);
    angle.y += (mousePositionDelta.x * -rotSensitity.x);

    angle.x = std::clamp(angle.x, xAngleClampMin * DEG2RAD, xAngleClampMax * DEG2RAD);
    angle.y = glm::wrapAngle(angle.y);

    lookAtAngle(angle);
  }

  void Editor::Camera::Update(float dt) {
    float speedMultiplier = movementSpeedMultiplier;

    if (IsKeyDown(KEY_LEFT_SHIFT))
      speedMultiplier *= boostMultiplier;

    auto [upChange, fwdChange, rgtChange] =
        rtCamera.getScaledDirectionVectors(dt * speedMultiplier);

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      HideCursor();
      preHideCursorLocation = GetMousePosition();
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      if (IsKeyDown(KEY_SPACE)) {
        rtCamera.lookFrom += upChange;
        rtCamera.lookAt += upChange;
      }
      if (IsKeyDown(KEY_LEFT_CONTROL)) {
        rtCamera.lookFrom -= upChange;
        rtCamera.lookAt -= upChange;
      }
      if (IsKeyDown(KEY_W)) {
        rtCamera.lookFrom += fwdChange;
        rtCamera.lookAt += fwdChange;
      }
      if (IsKeyDown(KEY_S)) {
        rtCamera.lookFrom -= fwdChange;
        rtCamera.lookAt -= fwdChange;
      }
      if (IsKeyDown(KEY_A)) {
        rtCamera.lookFrom -= rgtChange;
        rtCamera.lookAt -= rgtChange;
      }
      if (IsKeyDown(KEY_D)) {
        rtCamera.lookFrom += rgtChange;
        rtCamera.lookAt += rgtChange;
      }

      MouseLook(GetMouseDelta());
      SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
      SetMousePosition(preHideCursorLocation.x, preHideCursorLocation.y);
      ShowCursor();
    }
  }

  Camera3D Editor::Camera::toRaylibCamera3D() const {
    return Camera3D{.position = rtCamera.lookFrom,
                    .target = rtCamera.lookAt,
                    .up = {0, 1, 0},
                    .fovy = rtCamera.vFov,
                    .projection = CAMERA_PERSPECTIVE};
  }

  glm::mat4 Editor::Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(rtCamera.vFov),
                            static_cast<float>(editorWidth) / editorHeight,
                            0.01f, 100.0f);
  }

  float Editor::Camera::GetCorrectedCropFov() const {
    // Clamp the scale in case the image is larger than the editor window.
    auto const scale = std::min(static_cast<float>(_imageHeight) / editorHeight, 1.0f);
    return glm::degrees(2 * std::atan(scale * std::tan(DegressToRadians(rtCamera.vFov) / 2)));
  }

  rt::Camera Editor::Camera::toSceneCamera() const {
    auto const aspectRatio = static_cast<float>(_imageWidth) / _imageHeight;
    auto       vFov        = rtCamera.vFov;

    if (frameSizeAffectsCrop) {
      vFov = GetCorrectedCropFov();
    }

    return rt::Camera(rtCamera.lookFrom, rtCamera.lookAt, rtCamera.worldUp, vFov, aspectRatio, rtCamera.aperature,
                      rtCamera.focusDist, rtCamera.time0, rtCamera.time1);
  }
} // namespace rt
