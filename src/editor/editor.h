#pragma once

#include "../Defs.h"
#include "../IState.h"
#include "../Scene.h"
#include "../materials/Material.h"
#include "RenderAsync.h"
#include "app.h"
#include "camera.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <raylib.h>

#include <../vendor/glm/glm/glm.hpp>
#include <../vendor/glm/glm/gtx/fast_trigonometry.hpp>

#include <optional>

class Vector2;

namespace rt {
  class Hittable;
  class HittableList;
  class Scene;

  class Editor : public IState {
  public:

    struct ViewState {
      struct FileMenu {
        bool shouldOpen{false};
      };

      struct ViewMenu {
        bool shouldOpen{false};
        bool raytracingSettings{true}, cameraSettings{true}, objectList{true};
      };

      struct BuiltInMenu {
        bool shouldOpen{false};
      };

      FileMenu fileMenu;
      ViewMenu viewMenu;
      BuiltInMenu builtInMenu;
    };

    Editor(App *const app, CliConfig const &config, Scene const &initialScene)
        : IState(app),
          camera(config.editorWidth, config.editorHeight, config.imageWidth, config.imageHeight, initialScene),
          rasterRT(LoadRenderTexture(config.editorWidth, config.editorHeight)) {}

    ~Editor() { UnloadRenderTexture(rasterRT); }

    // Calls overloaded `Rasterize()` function for each object
    void Rasterize();

    void SelectedObjectGizmo();

    // Checks for editor related inputs (mouse clicks for picking, gizmo modes)
    void CheckInput();

    // Renders editor specific GUI
    void RenderImgui();

    void FileMenuImGui();

    void FileMenuCheckInputs();
    void ViewMenuCheckInputs();
    void ViewMenuImGui();

    void BuiltInMenuImGui();
    void BuiltInMenuCheckInputs();

    void SelectedObjectImGui();

    // Tests a ray against the world and returns whatever it hits (can be null)
    Hittable *CastRay(Vector2 mousePos);

    void AddObjectImgui();

    /*
      Checks for inputs, updates the editor camera and its GUI,
      rasterizes object, renders editor GUI.
    */
    virtual void onUpdate() override;

    virtual void onEnter() override;

    virtual void onExit() override;

    void RenderViewport();

    void RaytraceSettingsImgui();

    void ObjectListImgui();

    static std::optional<sPtr<Material>> MaterialChanger();

    void TopMenuImgui();

    virtual void changeScene(Scene *scene) override;

  private:
    Hittable *selectedObject = nullptr;

    enum AddableObjectsTypes { Box, Sphere, Plane, AddableObjectsTypesCount };
    inline static const char *addableObjectTypes[] = {
        "Box",
        "Sphere",
        "Plane",
    };

    AddableObjectsTypes selectedAddableObject = Box;

    ImGuizmo::MODE      imguizmoMode = ImGuizmo::MODE::LOCAL;
    ImGuizmo::OPERATION imguizmoOp   = ImGuizmo::OPERATION::TRANSLATE;

    inline static const Color colors[] = {
        GOLD,   GRAY,      DARKPURPLE, SKYBLUE,  MAROON, ORANGE, GREEN, RED,       DARKGRAY, PURPLE, LIGHTGRAY, BEIGE,
        VIOLET, DARKBROWN, MAGENTA,    RAYWHITE, LIME,   WHITE,  BROWN, DARKGREEN, DARKBLUE, BLUE,   YELLOW,    PINK,
    };

    static const int numColors = sizeof(colors) / sizeof(colors[0]);

    editor::Camera camera;
    RenderTexture2D rasterRT;

    ViewState viewState;

  }; // namespace Editor
} // namespace rt
