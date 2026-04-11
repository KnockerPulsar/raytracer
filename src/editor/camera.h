#pragma once

#include "common/Camera.h"
#include "scene.h"

namespace rt::editor {
  class Scene;
  class Camera: public Scene::Camera {
  public:
    enum class ControlType : uint8_t { flyCam, lookAtPoint, controlTypesCount };

    inline static vec3 lineStart, lineEnd;

    Camera(glm::ivec2 editorSize, glm::ivec2 imageSize, editor::Scene::Camera const &initialSceneCamera);

    void lookAtAngle(vec3 angle);
    void forwardToAngle(vec3 fwd);

    void MouseLook(Vector2 mousePositionDelta);

    void DrawFrameOutline() const;
    void RenderImgui();

    void Update(float dt);

    Camera3D toRaylibCamera3D() const;

    vec3 getLookFrom() const { return lookFrom; }
    vec3 localForward() const { return (lookAt - lookFrom).Normalize(); }
    float focusDist() const { return focusDist_; }

    glm::mat4 getProjectionMatrix() const;

    std::tuple<vec3, vec3, vec3> getScaledDirectionVectors(float dt) const;

    auto imageSize() const -> glm::ivec2 { return _imageSize; }
    auto editorSize() const -> glm::ivec2 { return editorSize_; }

    float GetCorrectedCropFov() const;

    rt::Camera toRaytracerCamera() const;


  private:
    glm::ivec2 editorSize_; // TODO should be const but that deletes `editor::Scene`'s copy ctor
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

    const static inline vec3 worldUp{0, 1, 0};
  };
} // namespace rt::editor
