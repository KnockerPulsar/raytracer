#include "editor.h"

#include "Utils.h"
#include "hittable.h"
#include "hittatable_builder.h"

#include "common/Constants.h"

#include "materials/diffuse_light.h"
#include "materials/lambertian.h"
#include "materials/dielectric.h"
#include "materials/metal.h"

// #include "objects/box.h"
// #include "objects/sphere.h"
// #include "objects/plane.h"

#include "raytracer/raytracer.h"
#include "raytracer/background.h"
#include "common/bvh_tree.h"
#include "common/Scene.h"

#include <ImGuiFileDialog.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <raylib.h>
#include <rlImGui.h>
#include <rlgl.h>

#include <algorithm>
#include <string>

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

    BeginMode3D(camera.toRaylibCamera3D());
    {
      ClearBackground(PURPLE); // So errors in drawing the background stand out.

      scene->getBackground().drawBackground(camera.getLookFrom());
      DrawGrid(10, 10);

      // if (!getScene()->skysphereTexture.empty()) {
      //   getScene()->skysphere->transformation.setTranslation(camera.getLookFrom());
      //   getScene()->drawSkysphere();
      // }

      auto const world = scene->getWorld();
      for (int i = 0; i < world.size(); i++) {
        world[i]->RasterizeTransformed(world[i]->getTransformation(), vec3(colors[i % numColors]));
      }

      // TODO bounding box visualizations

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
            auto const imageSize = scene->imageSize();
            scene = loader(imageSize.x, imageSize.y);
            // Re-initialize the camera.
            camera = Camera(camera.editorSize(), scene->imageSize(), scene->getCamera());
          }
        }
      }

      ImGui::EndMenu();
    }
  }

  void Editor::SelectedObjectImGui() {
    if (selectedObject != nullptr) {
      ImGui::Begin("Selected object", 0);

      SelectedObjectGizmo();

      // const auto selectedObjectUniqueName = selectedObject->name + "##" + EditorUtils::GetIDFromPointer(selectedObject);
      // ImGui::Button(selectedObjectUniqueName.c_str(), {-1, 0});
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

  editor::Object *Editor::CastRay(Vector2 mousePos) {

    ::Ray raylibRay = GetMouseRay(mousePos, camera.toRaylibCamera3D());
    Ray   r         = {raylibRay.position, raylibRay.direction, 0};

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

  Editor::Editor(App *const app, CliConfig const &config)
      : IState(app),
        scene(std::make_unique<editor::Scene>(config.pathToScene.empty()
                                                  ? editor::Scene::Earth(config.imageSize.x, config.imageSize.y)
                                                  : editor::Scene::Load(config.imageSize, config.pathToScene))),
        camera(config.editorSize, config.imageSize, scene->getCamera()),
        rasterRT(LoadRenderTexture(config.editorSize.x, config.editorSize.y)), config(config, *scene) {}

  Editor::~Editor() { UnloadRenderTexture(rasterRT); }

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
                 addableObjectTypes, AddableObjectsTypes::AddableObjectsTypesCount, -1);
    ImGui::SameLine();
    if (ImGui::Button("+", {-1, 0})) {

      sPtr<Hittable> added = [this] -> std::shared_ptr<Hittable> {
        const sPtr<Material> defaultMaterial = std::make_shared<DiffuseLight>(vec3(10, 0, 10));
        // TODO: This assumes that the root changes when we add new children, which might not be the case.
        // const auto rootID = EditorUtils::GetIDFromPointer(getScene()->worldRoot.get());

        switch (selectedAddableObject) {
        // case Box:
        //   return HittableBuilder<editor::Box>(1).withMaterial(defaultMaterial).withName("Added Box##" + rootID);
        //
        // case Sphere:
        //   return HittableBuilder<editor::Sphere>(1, defaultMaterial)
        //       .withMaterial(defaultMaterial)
        //       .withName("Added Sphere##" + rootID);
        //
        // case Plane:
        //   return HittableBuilder<editor::Plane>(1, 1).withMaterial(defaultMaterial).withName("Added Plane##" + rootID);

        default:
          return nullptr;
        }
      }();

      // /*auto *const newRoot =*/ getScene()->worldRoot->add(added);

      // if (newRoot == nullptr) {
      //   std::cerr << "Attempting to add a child to a world root that's not a BVHNode or a HittableList\n";
      // } else {
      //   getScene()->worldRoot = newRoot;
      // }
    }
  }

  // TODO this probably fits better in editor::Scene
  // Would require passing the editor camera, but would not require us to have
  // a getter for the raytracer background.
  auto Editor::compileScene() const -> std::unique_ptr<raytracer::Scene> {
    auto const world = scene->getWorld();

    // TODO can this store unique pointers instead?
    std::vector<std::shared_ptr<rt::Raytracable>> raytracables;
    raytracables.reserve(world.size());

    for (auto const &obj : world) {
      raytracables.push_back(obj->toRaytracable());
    }

    return std::make_unique<raytracer::Scene>(std::make_unique<raytracer::BVHTree>(std::move(raytracables), 0, 1),
                                              camera.toRaytracerCamera(), camera.imageSize(),
                                              scene->getBackground().toRaytracerBackground());
  }

  void Editor::onExit() {
    auto raytracerConfig = raytracer::Config{
        compileScene(),
        config.seed,
        config.numSamples,
        config.maxDepth,
        config.numThreads,
        camera.imageSize(),
    };

    rebuildRaytracer(
        std::make_shared<rt::raytracer::Raytracer>(app, std::move(raytracerConfig), config.saveOnRender, config.editorSize));
  }

  void Editor::RenderViewport() const {
    // Flip on Y axis since ImGui and opengl/raylib use different axis
    DrawTextureRec(rasterRT.texture, {0, 0, float(rasterRT.texture.width), -float(rasterRT.texture.height)}, {0, 0},
                   WHITE);
  }

  void Editor::ObjectListImgui() {
    auto& world = scene->getWorld();
    for (auto i = 0u; i < world.size(); i++) {
      auto const& o = world[i];
      auto const selected = selectedObject == o.get();

      if(selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorConvertFloat4ToU32({0.6, 0.4, 0.3, 1.0}));
        ImGui::SetScrollHereY();
      }

      std::string idPlusName = o->getName() + "##" + EditorUtils::GetIDFromPointer(o.get());
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
} // namespace rt
