#pragma once

#include "common/Camera.h"

namespace rt::editor {
  class Scene;
  class Camera {
  public:
    enum class ControlType { flyCam, lookAtPoint, controlTypesCount };

    inline static vec3 lineStart, lineEnd;

    Camera(vec3 lookFrom, vec3 lookAt, vec3 vUp, float vFov, float aspectRatio, float aperature, float focusDist); // TODO

    void updateFromRtCamera(rt::Camera const &sceneCamera);

    void UpdateRtCamera();

    void Fwd(float deltaTime);

    void Bck(float deltaTime);

    void lookAtAngle(vec3 angle);

    void forwardToAngle(vec3 fwd);

    void MouseLook(Vector2 mousePositionDelta);

    void DrawFrameOutline() const;

    void RenderImgui();

    void Update(float dt);

    Camera3D toRaylibCamera3D() const;

    vec3 getLookFrom() const { return rtCamera.lookFrom; }

    vec3 localForward() const { return (rtCamera.lookAt - rtCamera.lookFrom).Normalize(); }

    float focusDist() const { return rtCamera.focusDist; }

    glm::mat4 getProjectionMatrix() const;

    rt::Camera getRtCamera() const { return rtCamera; }

    glm::ivec2 imageSize() const { return _imageSize; }

    float GetCorrectedCropFov() const;

    rt::Camera toSceneCamera() const;

  private:
    rt::Camera rtCamera;

    glm::ivec2 const editorSize;
    glm::ivec2 _imageSize;

    vec3 moveDir;

    Vector2 preHideCursorLocation;
    bool    frameSizeAffectsCrop{false};

    constexpr static const float xAngleClampMin          = -89.0f;
    constexpr static const float xAngleClampMax          = 89.0f;
    float                        panningDivider          = 51.0f;
    float                        movementSpeedMultiplier = 10.0f;
    float                        boostMultiplier         = 5.0f;
    Vector2                      rotSensitity            = {0.003f, 0.003f};
    vec3                         angle; // Used to rotate the camera using the mouse

    ControlType               controlType         = ControlType::flyCam;
    inline static const char *controlTypeLabels[] = {"flyCam", "lookAt"};
  };
} // namespace rt::editor
