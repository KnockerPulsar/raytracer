#define IMGUI_DEFINE_MATH_OPERATORS
#include "editor.h"

#include "Utils.h"
#include "hittable.h"
#include "hittatable_builder.h"

#include "common/Constants.h"

#include "materials/diffuse_light.h"
#include "materials/lambertian.h"
#include "materials/dielectric.h"
#include "materials/metal.h"

#include "objects/box.h"
#include "objects/sphere.h"
#include "objects/plane.h"
#include "objects/mesh.h"

#include "raytracer/raytracer.h"

#include <ImGuiFileDialog.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>

#include <raylib.h>
#include <rlImGui.h>
#include <rlgl.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <print>

using namespace rt::constants;

namespace rt::editor {
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
      {"Toggle Raytracing settings",
       [](auto &viewState) { viewState.viewMenu.raytracingSettings = !viewState.viewMenu.raytracingSettings; },
       "ALT+R",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_R},
      {"Toggle Camera settings",
       [](auto &viewState) { viewState.viewMenu.cameraSettings = !viewState.viewMenu.cameraSettings; },
       "ALT+C",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_C},
      {"Toggle Object list",
       [](auto &viewState) { viewState.viewMenu.objectList = !viewState.viewMenu.objectList; },
       "ALT+O",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_O},
      {"Hide all",
       [](auto &viewState) { viewState.viewMenu = {false, false, false, false}; },
       "ALT+H",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_H},
      {"Show all",
       [](auto &viewState) { viewState.viewMenu = {true, true, true, true}; },
       "ALT+S",
       {KEY_LEFT_ALT, KEY_RIGHT_ALT},
       KEY_S},
  };

  void Editor::Rasterize() {
    BeginTextureMode(rasterRT);

    BeginMode3D(camera.toRaylibCamera3D());
    {
      ClearBackground(PURPLE); // So errors in drawing the background stand out.

      scene->getBackground().drawBackground(camera.getLookFrom());
      DrawGrid(10, 10);

      auto const world = scene->getWorld();
      for (int i = 0; i < world.size(); i++) {
        world[i]->RasterizeTransformed(world[i]->getTransformation(), vec3(colors[i % numColors]));
      }

      // TODO bounding box visualizations


      // TODO better focus distance visualization?
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

    auto* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    auto const windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("Dockspace", nullptr, windowFlags);

    TopMenuImgui();

    auto const dockspaceId = ImGui::GetID("Dockspace");
    ImGui::DockSpace(dockspaceId, ImVec2{0.0f, 0.0f}, ImGuiDockNodeFlags_None);

    RenderViewport();

    if (viewState.viewMenu.objectList) {
      ImGui::Begin("Objects");
      {
        AddObjectImgui();
        ObjectListImgui();
      }
      ImGui::End();
    }

    if (viewState.viewMenu.raytracingSettings)
      config.OnImgui();

    if (viewState.viewMenu.cameraSettings)
      camera.RenderImgui();

    SelectedObjectImGui();

    ImGui::End();
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

      // if (ImGuiFileDialog::Instance()->IsOk()) {
      //   std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      //   app->changeScene(Scene::Load(getScene()->imageSize, filePathName));
      // }

      ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("SaveScene")) {

      if (ImGuiFileDialog::Instance()->IsOk()) {

        std::string fileDir  = ImGuiFileDialog::Instance()->GetCurrentPath();
        std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

        std::stringstream sceneNamePlusTime;
        sceneNamePlusTime << fileDir << "/" << fileName;

        // TODO
        // json          json = getScene()->toJson();
        // std::ofstream outputFile(sceneNamePlusTime.str());
        // outputFile << std::setw(4) << json << '\n';
        // outputFile.close();
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
          {
            auto const imageSize = camera.imageSize();
            ChangeScene(loader(imageSize.x, imageSize.y));
          }
        }
      }

      ImGui::EndMenu();
    }
  }

  void Editor::SelectedObjectImGui() {
    if (selectedObject != nullptr) {
      ImGui::Begin("Selected object", 0);

      // TODO double clicks should allow rename
      const auto selectedObjectUniqueName = selectedObject->getName() + "##" + EditorUtils::GetIDFromPointer(selectedObject);
      ImGui::Button(selectedObjectUniqueName.c_str(), {-1, 0});

      // TODO Button for centering object in screen
      //  Momentary and persistent
      // TODO Button to set focal distance to object distance
      selectedObject->getTransformation().OnImgui();
      ImGui::Spacing();
      selectedObject->OnImgui();
      ImGui::Spacing();
      selectedObject->getMaterial().OnImgui();

      ImGui::End();
    }
  }

  void Editor::SelectedObjectGizmo() {
    assert(selectedObject != nullptr);

    ImGuizmo::BeginFrame();

    assert(viewport.window != nullptr);
    ImGuizmo::SetAlternativeWindow(viewport.window);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, viewport.size.x, viewport.size.y);

    auto const rotation = selectedObject->getTransformation().getRotationEuler();

    auto model = [this, rotation] {
      float                 scaleTemp[] = {1.0, 1.0, 1.0};
      std::array<float, 16> model;
      auto const            translation = selectedObject->getTransformation().getTranslation();
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

      selectedObject->getTransformation().setTranslation(translation);
      selectedObject->getTransformation().rotateDelta(rotationDelta);
    }
  }

  void Editor::CheckInput() {
    FileMenuCheckInputs();
    ViewMenuCheckInputs();
    BuiltInMenuCheckInputs();
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
        if (ImGui::MenuItem(text.c_str(), strShortcut.c_str())) {
          onPress(viewState);
        }
      }
      ImGui::EndMenu();
    }
  }

  editor::Object *Editor::CastRay(Vector2 clickUv) {
    // Raytracing and the editor have flipped Y axis
    auto const r = camera.toRaytracerCamera(Camera::Target::editor).GetRay(clickUv.x, 1.0f - clickUv.y);

    editor::Camera::lineStart = r.origin.toGlm();
    editor::Camera::lineEnd   = r.At(1000).toGlm();

    auto const &world = scene->getWorld();
    auto        closestT      = rt::constants::infinity;
    Object     *closestObject = nullptr;

    for (auto const &obj : world) {
      if (auto const t = obj->hit(r)) {
        if (t < closestT) {
          closestT      = *t;
          closestObject = obj.get();
        }
      }
    }

    return closestObject;
  }

  auto loadInitialScene(CliConfig const &config) -> std::unique_ptr<Scene> {
    if (!config.initialScene.empty() && !config.pathToScene.empty())
      // TODO proper logging?
      std::cerr << "Both built in scene and path to scene specified, loading scene from path...\n";

    if (!config.pathToScene.empty())
      return std::make_unique<editor::Scene>(Scene::Load(config.pathToScene));

    if (!config.initialScene.empty()) {
      auto const &sceneBuilder = std::ranges::find_if(
          Scene::builtInScenes, [&](auto const &pair) { return pair.first == config.initialScene; });

      return sceneBuilder->second(config.imageSize.x, config.imageSize.y);
    }

    std::unreachable();
  }

  Editor::Editor(App *const app, CliConfig const &config)
      : IState(app), scene(loadInitialScene(config)), camera(config.windowSize, config.imageSize, scene->getCamera()),
        rasterRT(LoadRenderTexture(config.windowSize.x, config.windowSize.y)), config(config, *scene) {}

  Editor::~Editor() { UnloadRenderTexture(rasterRT); }

  void Editor::onUpdate() {
    CheckInput();

    if(raytracer)
      raytracer->onUpdate();

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
                 addableObjectTypes, AddableObjectsTypes::AddableObjectsTypesCount, -1);
    ImGui::SameLine();
    if (ImGui::Button("+", {-1, 0})) {

      sPtr<Object> added = [this] -> std::shared_ptr<Object> {
        const sPtr<Material> defaultMaterial = std::make_shared<DiffuseLight>(vec3(10, 0, 10));
        auto constexpr defaultMeshPath       = "assets/models/nefertiti-5k.obj";

        // TODO: This assumes that the root changes when we add new children, which might not be the case.
        const auto rootID = EditorUtils::GetIDFromPointer(scene.get());

        switch (selectedAddableObject) {
        case Box:
          return HittableBuilder<editor::Box>(1).withMaterial(defaultMaterial).withName("Added Box##" + rootID);

        case Sphere:
          return HittableBuilder<editor::Sphere>(1).withMaterial(defaultMaterial).withName("Added Sphere##" + rootID);

        case Plane:
          return HittableBuilder<editor::Plane>(1, 1).withMaterial(defaultMaterial).withName("Added Plane##" + rootID);

        case Mesh:
          return HittableBuilder<editor::Mesh>(defaultMeshPath)
              .withMaterial(defaultMaterial)
              .withName("Added Mesh##" + rootID);

        default:
          return nullptr;
        }
      }();

      scene->getWorld().push_back(added);
    }
  }

  void Editor::onExit() {}

  void Editor::RenderViewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport");

    viewport = {ImGui::GetCurrentWindow(), ImGui::GetContentRegionAvail()};

    if (rasterRT.texture.width != viewport.size.x || rasterRT.texture.height != viewport.size.y)
    {
      UnloadRenderTexture(rasterRT);
      rasterRT = LoadRenderTexture(viewport.size.x, viewport.size.y);
      camera.updateViewportSize({viewport.size.x, viewport.size.y});
    }

    // Flip on Y axis since ImGui and us use flipped vertical axes.
    ImGui::Image(reinterpret_cast<ImTextureID>(&rasterRT.texture.id), viewport.size, ImVec2{0, 1}, ImVec2{1, 0});

    if (ImGui::IsWindowHovered()) {
      camera.Update(GetFrameTime());

      if (ImGui::IsWindowHovered() && IsMouseButtonPressed(0)) {
        auto const viewportOffset   = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
        auto const mousePosRelative = ImGui::GetMousePos() - viewportOffset;

        auto const clickUv = mousePosRelative / viewport.size;
        selectedObject     = CastRay({clickUv.x, clickUv.y});
      }

    }

    if (selectedObject != nullptr) {
      if (ImGui::IsKeyPressed(ImGuiKey_R)) {
        imguizmoOp = ImGuizmo::OPERATION::ROTATE;
      }
      if (ImGui::IsKeyPressed(ImGuiKey_T)) {
        imguizmoOp = ImGuizmo::OPERATION::TRANSLATE;
      }
      if (ImGui::IsKeyPressed(ImGuiKey_Y)) {
        imguizmoOp = ImGuizmo::OPERATION::SCALE;
      }
      if (ImGui::IsKeyPressed(ImGuiKey_X)) {
        imguizmoMode = imguizmoMode == ImGuizmo::MODE::LOCAL ? ImGuizmo::MODE::WORLD : ImGuizmo::MODE::LOCAL;
      }
      SelectedObjectGizmo();
    }

    ImGui::End();
    ImGui::PopStyleVar();

    renderedFramesWindow.draw();

    ImGui::Begin("Raytrace Controls", nullptr, ImGuiWindowFlags_NoTitleBar);
    if (ImGui::Button("Start Raytracing")) {
      auto raytracerConfig = raytracer::Config{
          scene->compile(camera.toRaytracerCamera()),
          config.seed,
          config.numSamples,
          config.maxDepth,
          config.numThreads,
          camera.imageSize(),
      };

      raytracer = std::make_unique<rt::raytracer::Raytracer>(app, std::move(raytracerConfig), [this](auto frame) {
        renderedFramesWindow.onFrameRendered(frame);
        ImGui::SetWindowFocus("Rendered Frames");
      });

      raytracer->startRaytracing();
    }

    ImGui::SameLine();
    if (ImGui::Button("Stop Raytracing")) {
      raytracer->stopRaytracing();
    }

    if (raytracer && raytracer->isRaytracing()) {
      ImGui::SameLine();
      auto const progress = raytracer->progress();

      if (progress < 1.0f)
        ImGui::ProgressBar(progress * 100.0f);
    }

    ImGui::End();
  }

  void Editor::ObjectListImgui() {
    auto& world = scene->getWorld();
    for (auto i = 0u; i < world.size(); i++) {
      auto const& o = world[i];
      auto const selected = selectedObject == o.get();

      if(selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorConvertFloat4ToU32({0.6, 0.4, 0.3, 1.0}));
      }

      std::string idPlusName = o->getName() + "##" + EditorUtils::GetIDFromPointer(o.get());
      // TODO double clicks should allow renames
      if (ImGui::Button(idPlusName.c_str())) {
        selectedObject = o.get();
      }

      if (selected) {
        ImGui::PopStyleColor();
      }

      auto const checkboxWidth = ImGui::CalcTextSize("x").x;
      auto const totalWidth    = ImGui::GetContentRegionAvail().x;
      ImGui::SameLine(totalWidth - checkboxWidth);
      ImGui::SetNextItemWidth(totalWidth - checkboxWidth);

      // Remove current object from world and rebuild
      if (ImGui::Button(("x##" + EditorUtils::GetIDFromPointer(o.get())).c_str())) {
        world.erase(world.begin() + i);
      }

      ImGui::Separator();
    }
  }

  // TODO texture changer
  std::optional<sPtr<editor::Material>> Editor::MaterialChanger() {

    static const char   *materialTypes[]      = {"Diffuse", "Dielectric", "Metal", "Emissive"};

    enum class MaterialTypes : uint8_t {
      Diffuse,
      Dielectrical,
      Metallic,
      Emissive,
      MaterialTypesCount,
    };

    static MaterialTypes selectedMaterialType = MaterialTypes::Emissive;

    ImGui::Combo(("##" + EditorUtils::GetIDFromPointer(materialTypes)).c_str(), (int *)&selectedMaterialType,
                 materialTypes, static_cast<int>(MaterialTypes::MaterialTypesCount) - 1);

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

    if (ImGui::BeginMainMenuBar()) {

      FileMenuImGui();
      ViewMenuImGui();
      BuiltInMenuImGui();

      ImGui::EndMainMenuBar();
    }
  }

  void Editor::ChangeScene(std::unique_ptr<Scene> newScene) {
    camera.updateFromSceneCamera(newScene->getCamera());
    selectedObject = nullptr;

    scene = std::move(newScene);
  }
} // namespace rt
