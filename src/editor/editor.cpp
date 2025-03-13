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

#include <cstdio>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace rt {

  void Editor::Rasterize() {

    BeginTextureMode(rasterRT);
    ClearBackground(getScene()->backgroundColor.toRaylibColor(255));

    BeginMode3D(camera.toRaylibCamera3D());
    {
      DrawGrid(10, 10);

      if (!getScene()->skysphereTexture.empty()) {
        getScene()->skysphere->transformation.translate = camera.getLookFrom();
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

      DrawSphere(camera.getLookFrom() +
                     camera.localForward() * camera.focusDist(),
                 0.05f, LIME);

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

    RaytraceSettingsImgui();

    SelectedObjectGizmo();

    camera.RenderImgui();
  }

  void Editor::SelectedObjectGizmo() {
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

      ImGuizmo::Manipulate(glm::value_ptr(camera.getViewMatrix()),
                           glm::value_ptr(camera.getProjectionMatrix()),
                           imguizmoOp, imguizmoMode, model);

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

  void Editor::onEnter() { camera.updateFromRtCamera(getScene()->cam); }

  // Regenerate BVH on exit / before entering the raytracer
  void Editor::onExit() {
    auto *scene = getScene();
    scene->cam = camera.getRtCamera();

    scene->imageWidth = camera.imageWidth();
    scene->imageHeight = camera.imageHeight();

    *app->getARD() =
        AsyncRenderData(camera.imageWidth(), camera.imageHeight(), editorWidth,
                        editorHeight, app->getNumThreads());
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

      if (ImGui::BeginMenu("Scene")) {
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

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Built in")) {
        for (auto &[name, loader] : Scene::builtInScenes) {
          if (ImGui::MenuItem(name.c_str())) {
            app->changeScene(loader(imageWidth, imageHeight));
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
        app->changeScene(Scene::Load(imageWidth, imageHeight, filePathName));
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

        json          json = getScene()->toJson();
        std::ofstream outputFile(sceneNamePlusTime.str());
        outputFile << std::setw(4) << json << std::endl;
        outputFile.close();
      }

      // close
      ImGuiFileDialog::Instance()->Close();
    }
  }

  void Editor::changeScene(Scene *scene) {
    camera.updateFromRtCamera(scene->cam);
  }

  void Editor::Camera::DrawFrameOutline() const {
    auto rectX = (editorWidth - _imageWidth) / 2;
    auto rectY = (editorHeight - _imageHeight) / 2;
    DrawRectangleLines(rectX, rectY, _imageWidth, _imageHeight, MAGENTA);
  }

  void Editor::Camera::RenderImgui() {
    if (ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
      vec3 deltaPos = lookFrom;

      ImGui::DragFloat3("lookFrom", &lookFrom.x, 0.05);

      ImGui::Combo(
          "Camera type", (int *)&controlType, controlTypeLabels, Editor::Camera::ControlType::controlTypesCount, 0
      );

      if (controlType == ControlType::lookAtPoint)
        ImGui::DragFloat3("lookAt", &lookAt.x, 0.05);
      else {
        deltaPos -= lookFrom; // lookFrom = oldLookFrom + delta
	lookAt += (deltaPos);

        ImGui::DragFloat2("rotation", &angle.x, 0.05f);

	MouseLook({0, 0});
      }

      ImGui::DragFloat("Camera speed multiplier", &movScale, 0.1, 1, 100);

      ImGui::DragFloat("Camera boost multiplier", &movMultiplier, 0.1, 1, 10);

      ImGui::DragInt2("frame size", &_imageWidth);

      // Manual clamping since we want different limits for the width an height
      _imageWidth = std::min(_imageWidth, editorWidth);
      _imageHeight = std::min(_imageHeight, editorHeight);
    }

    ImGui::End();
  }

  Editor::Camera::Camera(int editorWidth, int editorHeight, int imageWidth,
                         int imageHeight, Scene const &initialScene)
      : editorWidth(editorWidth), editorHeight(editorHeight),
        _imageWidth(imageWidth), _imageHeight(imageHeight),
        rtCamera(initialScene.cam) {
    updateFromRtCamera(rtCamera);
    auto const fwd = localForward();

    angle.x = fwd.y;

    auto xzFwdProj = fwd.projectOntoPlane(vec3(0, 1, 0)).Normalize();
    auto cosAngley = vec3::DotProd(xzFwdProj, {0, 0, -1});
    auto signY = fwd.x < 0 ? 1 : -1;
    angle.y = signY * acos(cosAngley);
  }

  void Editor::Camera::updateFromRtCamera(rt::Camera const &sceneCamera) {
    rtCamera = sceneCamera;
    lookFrom = sceneCamera.lookFrom;
    lookAt = sceneCamera.lookAt;
    vFov = sceneCamera.vFov;
  }

  void Editor::Camera::UpdateRtCamera() {
    rtCamera = rt::Camera(lookFrom, lookAt, {0, 1, 0}, vFov,
                          static_cast<float>(_imageWidth) / _imageHeight, 0.1f,
                          80.0f, 0.0, 1.0);
  }

  void Editor::Camera::Fwd(float deltaTime) {
    lookFrom += moveDir * deltaTime;
    UpdateRtCamera();
  }
  void Editor::Camera::Bck(float deltaTime) {
    lookFrom += -moveDir * deltaTime;
    UpdateRtCamera();
  }

  void Editor::Camera::MouseLook(Vector2 mousePositionDelta) {
    angle.x += (mousePositionDelta.y * -rotSensitity.y);
    angle.y += (mousePositionDelta.x * -rotSensitity.x);

    angle.x =
        glm::clamp(angle.x, xAngleClampMin * DEG2RAD, xAngleClampMax * DEG2RAD);
    angle.y = glm::wrapAngle(angle.y);

    glm::mat4 rotMat = glm::eulerAngleYX(angle.y, angle.x);
    glm::vec4 defaultFwd = glm::vec4(0, 0, -1, 0);

    glm::vec3 rotatedFwd = rotMat * defaultFwd;

    lookAt = lookFrom + rotatedFwd;

    UpdateRtCamera();
  }

  void Editor::Camera::Update(float dt) {
    float speedMultiplier = 1.0f;

    if (IsKeyDown(KEY_LEFT_SHIFT))
      speedMultiplier *= movMultiplier;

    auto [upChange, fwdChange, rgtChange] =
        rtCamera.getScaledDirectionVectors(dt * speedMultiplier);

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      HideCursor();

      if (IsKeyDown(KEY_SPACE)) {
        lookFrom += upChange;
        lookAt += upChange;
      }
      if (IsKeyDown(KEY_LEFT_CONTROL)) {
        lookFrom -= upChange;
        lookAt -= upChange;
      }
      if (IsKeyDown(KEY_W)) {
        lookFrom += fwdChange;
        lookAt += fwdChange;
      }
      if (IsKeyDown(KEY_S)) {
        lookFrom -= fwdChange;
        lookAt -= fwdChange;
      }
      if (IsKeyDown(KEY_A)) {
        lookFrom -= rgtChange;
        lookAt -= rgtChange;
      }
      if (IsKeyDown(KEY_D)) {
        lookFrom += rgtChange;
        lookAt += rgtChange;
      }

      MouseLook(GetMouseDelta());
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
      ShowCursor();
    }
  }

  Camera3D Editor::Camera::toRaylibCamera3D() const {
    return Camera3D{.position = lookFrom,
                    .target = lookAt,
                    .up = {0, 1, 0},
                    .fovy = vFov,
                    .projection = CAMERA_PERSPECTIVE};
  }

  glm::mat4 Editor::Camera::getViewMatrix() const {
    // glm::mat4 M = glm::mat4(1);
    // M           = glm::translate(M, lookFrom.toGlm());
    // M           = M * glm::eulerAngleXYZ(angle.x, angle.y, angle.z) ;

    // glm::vec3 eye    = M * glm::vec4(0, 0, 0, 1);
    // glm::vec3 center = M * glm::vec4(0, 0, -1, 1);
    // glm::vec3 up     = M * glm::vec4(0, 1, 0, 0);

    return glm::lookAt(lookFrom.toGlm(), lookAt.toGlm(), vec3(0, 1, 0).toGlm());
  }

  glm::mat4 Editor::Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(vFov),
                            static_cast<float>(_imageWidth) / _imageHeight,
                            0.01f, 1000.0f);
  }
  } // namespace rt
