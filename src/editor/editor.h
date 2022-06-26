#pragma once
#include "../Defs.h"
#include "../IState.h"
#include "../Scene.h"
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
    Hittable       *selectedObject = nullptr;
    Camera         *cam;
    sPtr<rt::Scene> currentScene;
    RenderTexture2D screenRT;

    bool   mouseInsideEditorViewport = false;
    ImVec2 viewportMin, viewportMax, viewportSize;

    enum AddableObjectsTypes { Box, Sphere, Plane, AddableObjectsTypesCount };
    inline static const char *addableObjectTypes[] = {
        "Box",
        "Sphere",
        "Plane",
    };

    AddableObjectsTypes selectedAddableObject = Box;

    Editor(sPtr<Scene> s) : currentScene(s) { cam = &s->cam; }

    // Calls overloaded `Rasterize()` function for each object
    void Rasterize();

    // Renders editor specific GUI
    void RenderImgui();

    // If a selected object is available, renders a gizmo on it
    void SelectedObjectGizmo() const;

    // Checks for editor related inputs (mouse clicks for picking, gizmo modes)
    void CheckInput();

    // Tests a ray against the world and returns whatever it hits (can be null)
    Hittable *CastRay(Vector2 mousePos) const;

    void addObject();

    /*
      Checks for inputs, updates the editor camera and its GUI,
      rasterizes object, renders editor GUI.
    */
    virtual void onUpdate() override;

    virtual void onEnter() override {}

    virtual void onExit() override {}

    void UpdateViewportRect(float imguiWidth, float height);

    void RenderViewport();
  
  
    void RaytraceSettingsImgui();
  
  }; // namespace Editor
} // namespace rt