#pragma once
#include "imgui.h"
#include "ImGuizmo.h"
#include "../Defs.h"
#include "../IState.h"
#include "../Scene.h"
#include "../materials/Material.h"
#include "RenderAsync.h"
#include <optional>
#include <raylib.h>
#include <vector>

class Vector2;

namespace rt {
  class Hittable;
  class HittableList;
  class Camera;
  class Scene;

  class Editor : public IState {
  public:
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

    RenderTexture2D rasterRT;

    Editor(CliConfig const &config)
        : rasterRT(LoadRenderTexture(config.editorWidth, config.editorHeight)) {
    }

    ~Editor() { UnloadRenderTexture(rasterRT); }

    // Calls overloaded `Rasterize()` function for each object
    void Rasterize();

    // Renders editor specific GUI
    void RenderImgui();

    // If a selected object is available, renders a gizmo on it
    void SelectedObjectGizmo();

    // Checks for editor related inputs (mouse clicks for picking, gizmo modes)
    void CheckInput();

    // Tests a ray against the world and returns whatever it hits (can be null)
    Hittable *CastRay(Vector2 mousePos);

    void AddObjectImgui();

    /*
      Checks for inputs, updates the editor camera and its GUI,
      rasterizes object, renders editor GUI.
    */
    virtual void onUpdate() override;

    virtual void onEnter() override {}

    virtual void onExit() override;

    void RenderViewport();

    void RaytraceSettingsImgui();

    void ObjectListImgui();

    static std::optional<sPtr<Material>> MaterialChanger();

    void TopMenuImgui();

    virtual void changeScene(Scene *scene) override;

  }; // namespace Editor
} // namespace rt
