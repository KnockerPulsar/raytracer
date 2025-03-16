#pragma once

#include "../Defs.h"
#include "../IState.h"
#include "../Scene.h"
#include "../materials/Material.h"
#include "RenderAsync.h"
#include "app.h"

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
  class Camera;
  class Scene;

  class Editor : public IState {
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

    // TODO split out all editor specific camera state here
    class Camera
    {
    public:
      enum ControlType { flyCam, lookAtPoint, controlTypesCount };

      inline static vec3 lineStart, lineEnd;

      Camera(int editorWidth, int editorHeight, int imageWidth, int imageHeight,
             Scene const &initialScene);

      void updateFromRtCamera(rt::Camera const &sceneCamera);

      void UpdateRtCamera();

      void Fwd(float deltaTime);

      void Bck(float deltaTime);

      void MouseLook(Vector2 mousePositionDelta);

      void DrawFrameOutline() const;

      void RenderImgui();

      void Update(float dt);

      Camera3D toRaylibCamera3D() const;

      vec3 getLookFrom() const { return lookFrom; }

      vec3 localForward() const { return (lookAt - lookFrom).Normalize(); }

      float focusDist() const { return rtCamera.focusDist; }

      glm::mat4 getViewMatrix() const;

      glm::mat4 getProjectionMatrix() const;

      rt::Camera getRtCamera() const { return rtCamera; }

      int imageWidth() const { return _imageWidth; }
      int imageHeight() const { return _imageHeight; }

    private:
      rt::Camera rtCamera;

      int const editorWidth, editorHeight;
      int _imageWidth, _imageHeight;

      vec3 lookFrom, lookAt, moveDir;
      float vFov;

      constexpr static const float xAngleClampMin = -89.0f;
      constexpr static const float xAngleClampMax = 89.0f;
      float                        panningDivider = 51.0f;
      float                        movScale       = 10.0f;
      float                        movMultiplier  = 5.0f;
      vec3                         angle          = {0, -3.14, 0}; // Used to rotate the camera using the mouse
      Vector2                      rotSensitity   = {0.003f, 0.003f};

      ControlType               controlType         = ControlType::flyCam;
      inline static const char *controlTypeLabels[] = {"flyCam", "lookAt"};
    };

    Camera camera;
    RenderTexture2D rasterRT;
    int const editorWidth, editorHeight;

    struct ViewState {
      bool raytracingSettings{true}, cameraSettings{true}, objectList{true};
    } viewState;

  public:
    Editor(CliConfig const &config, Scene const &initialScene)
        : camera(config.editorWidth, config.editorHeight, config.imageWidth,
                 config.imageHeight, initialScene),
          rasterRT(LoadRenderTexture(config.editorWidth, config.editorHeight)),
          editorWidth(config.editorWidth), editorHeight(config.editorHeight) {}

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

    virtual void onEnter() override;

    virtual void onExit() override;

    void RenderViewport();

    void RaytraceSettingsImgui();

    void ObjectListImgui();

    static std::optional<sPtr<Material>> MaterialChanger();

    void TopMenuImgui();

    virtual void changeScene(Scene *scene) override;

  }; // namespace Editor
} // namespace rt
