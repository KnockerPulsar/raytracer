#pragma once
#include "Constants.h"
#include "Defs.h"
#include "IRasterizable.h"
#include "data_structures/vec3.h"
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <vector>

namespace rt {

  class Camera {
  public:
    vec3 lookFrom, lookAt, moveDir;
    vec3 lower_left_corner, horizontal, vertical;
    vec3 localRight, localUp, localForward, worldUp;

    float viewportWidth, viewportHeight;

    float time0, time1;
    float vFov, // vertical field-of-view in degrees
        aspectRatio, aperature, focusDist, lensRadius;

    constexpr static const float xAngleClampMin = -89.0f;
    constexpr static const float xAngleClampMax = 89.0f;
    float                        panningDivider = 51.0f;
    float                        movScale       = 10.0f;
    float                        movMultiplier  = 5.0f;
    vec3                         angle          = {0, 0, 0}; // Used to rotate the camera using the mouse
    Vector2                      rotSensitity   = {0.003f, 0.003f};

    enum ControlType { flyCam, lookAtPoint, controlTypesCount };
    ControlType               controlType         = ControlType::flyCam;
    inline static const char *controlTypeLabels[] = {"flyCam", "lookAt"};

    inline static vec3 lineStart, lineEnd;

    Camera() = default;

    Camera(vec3  lookFrom,
           vec3  lookAt,
           vec3  vUp,
           vec3  moveDir,
           float vFov,
           float aspectRatio,
           float aperature,
           float focusDist,
           float time0 = 0.0,
           float time1 = 1.0);

    Camera(nlohmann::json cameraJson, float aspectRatio);

    // Calculates horizontal, vertical, lower_left_corner, rgt from input data.
    void GenerateData();

    void Fwd(float deltaTime) {
      lookFrom += moveDir * deltaTime;
      GenerateData();
    }

    void Bck(float deltaTime) {
      lookFrom += -moveDir * deltaTime;
      GenerateData();
    }

    Camera3D toRaylibCamera3D() const {
      return Camera3D{
          .position = lookFrom, .target = lookAt, .up = worldUp, .fovy = vFov, .projection = CAMERA_PERSPECTIVE};
    }

    Ray GetRay(float s, float t) const;

    void                         Update(float dt);
    void                         MouseLook(Vector2 mousePositionDelta);
    void                         RenderImgui();
    std::tuple<vec3, vec3, vec3> getScaledDirectionVectors(float dt) const;

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void UpdateDirectionVectors();
  };

  inline void to_json(json &j, const Camera &c) {
    j = json{
        "camera",
        {
            {"look_from", c.lookFrom},
            {"look_at", c.lookAt},
            {"v_up", c.worldUp},
            {"focus_dist", c.focusDist},
            {"aperature", c.aperature},
            {"move_dir", c.moveDir},
            {"fov", c.vFov},
            {"time0", c.time0},
            {"time1", c.time1},
        },
    };
  }
} // namespace rt
