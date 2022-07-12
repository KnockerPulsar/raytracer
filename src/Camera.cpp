#include "Camera.h"
#include "../vendor/rlImGui/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"
#include "Util.h"
#include <cmath>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <tuple>
#include <vector>

#include "../vendor/glm/glm/ext/matrix_clip_space.hpp"
#include "../vendor/glm/glm/gtc/matrix_transform.hpp"
#include "../vendor/glm/glm/gtx/fast_trigonometry.hpp"

#include "editor/Utils.h"
#include "rt.h"

namespace rt {

  Camera::Camera(
      vec3  lookFrom,
      vec3  lookAt,
      vec3  vUp,
      vec3  moveDir,
      float vFov,
      float aspectRatio,
      float aperature,
      float focusDist,
      float time0,
      float time1
  )
      : lookFrom(lookFrom), lookAt(lookAt), worldUp(vUp), moveDir(moveDir), vFov(vFov), aspectRatio(aspectRatio),
        aperature(aperature), focusDist(focusDist), time0(time0), time1(time1) {

    // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
    GenerateData();

    auto yzFwdProj = localForward.projectOntoPlane(vec3(1, 0, 0)).Normalize();
    auto xzFwdProj = localForward.projectOntoPlane(vec3(0, 1, 0)).Normalize();

    // auto cosAnglex = localForward.y;
    auto cosAngley = vec3::DotProd(xzFwdProj, {0, 0, -1});
    auto signY     = localForward.x < 0? 1 : -1;

    // angle.x = signX * acos(cosAnglex);
    angle.x = localForward.y;
    angle.y = signY * acos(cosAngley);

    lensRadius    = aperature / 2;
    this->lookAt  = lookAt;
    this->moveDir = moveDir;
  }

  // Should probably be moved to from_json()?
  Camera::Camera(nlohmann::json cameraJson, float aspectRatio)
      : Camera(
            cameraJson["look_from"].get<vec3>(),
            cameraJson["look_at"].get<vec3>(),
            cameraJson["v_up"].get<vec3>(),
            cameraJson["move_dir"].get<vec3>(),
            cameraJson["fov"].get<float>(),
            aspectRatio,
            cameraJson["aperature"].get<float>(),
            cameraJson["focus_dist"].get<float>(),
            cameraJson["time0"].get<float>(),
            cameraJson["time1"].get<float>()
        ) {
    controlType = cameraJson["type"] == "flycam" ? ControlType::flyCam : ControlType::lookAtPoint;
  }

  void Camera::GenerateData() {
    float theta = DegressToRadians(vFov);

    // https://raytracing.github.io/images/fig-1.14-cam-view-geom.jpg
    float h        = tan(theta / 2);
    viewportHeight = 2.0 * h;
    viewportWidth  = aspectRatio * viewportHeight;
    lensRadius     = aperature / 2;

    UpdateDirectionVectors();
  }

  Camera3D Camera::toRaylibCamera3D() const {
    return Camera3D{
        .position = lookFrom, .target = lookAt, .up = worldUp, .fovy = vFov, .projection = CAMERA_PERSPECTIVE};
  }

  void Camera::UpdateDirectionVectors() {
    // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
    localForward      = (lookAt - lookFrom).Normalize();                   // back vector
    localRight        = -vec3::CrsProd(worldUp, localForward).Normalize(); // Right vector
    localUp           = -vec3::CrsProd(localForward, localRight); // Local up vector, vUp is the world up vector
    horizontal        = focusDist * viewportWidth * localRight;
    vertical          = focusDist * viewportHeight * localUp;
    lower_left_corner = this->lookFrom - horizontal / 2 - vertical / 2 + focusDist * localForward;
  }

  // Used for raytracing
  Ray Camera::GetRay(float s, float t) const {
    vec3 rd     = lensRadius * vec3::RandomInUnitDisc();
    vec3 offset = localRight * rd.x + localUp * rd.y;
    return Ray(
        lookFrom + offset,
        (lower_left_corner + horizontal * s + vertical * t - lookFrom - offset).Normalize(),
        RandomFloat(time0, time1)
    );
  }

  void Camera::MouseLook(Vector2 mousePositionDelta) {
    angle.x += (mousePositionDelta.y * -rotSensitity.y);
    angle.y += (mousePositionDelta.x * -rotSensitity.x);

    angle.x = glm::clamp(angle.x, xAngleClampMin * DEG2RAD, xAngleClampMax * DEG2RAD);
    angle.y = glm::wrapAngle(angle.y);

    glm::mat4 rotMat     = glm::eulerAngleYX(angle.y, angle.x);
    glm::vec4 defaultFwd = glm::vec4(0, 0, -1, 0);

    glm::vec3 rotatedFwd = rotMat * defaultFwd;

    lookAt = lookFrom + rotatedFwd;

    UpdateDirectionVectors();
  }

  void Camera::RenderImgui() {
    if (ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
      vec3 deltaPos = lookFrom;
      ImGui::DragFloat3("lookFrom", &lookFrom.x, 0.05);

      ImGui::Combo(
          "Camera type", (int *)&controlType, controlTypeLabels, rt::Camera::ControlType::controlTypesCount, 0
      );

      if (controlType == rt::Camera::ControlType::lookAtPoint)
        ImGui::DragFloat3("lookAt", &lookAt.x, 0.05);
      else {
        deltaPos -= lookFrom; // lookFrom = oldLookFrom + delta
        lookAt += (deltaPos);
        ImGui::DragFloat2("rotation", &angle.x, 0.05f);
        MouseLook({0, 0});
      }

      ImGui::DragFloat("Camera speed multiplier", &movScale, 0.1, 1, 100);

      ImGui::DragFloat("Camera boost multiplier", &movMultiplier, 0.1, 1, 10);

      ImGui::DragFloat("Vertical FOV", &vFov, 0.1f, 20, 180);

      ImGui::DragFloat("Focus distance", &focusDist, 0.01, 0.1, 1000);

      ImGui::DragFloat("Aperature", &aperature, 0.01, 10e-6, 10);
    }
    ImGui::End();
  }

  void Camera::Update(float dt) {
    GenerateData();

    bool keyPressed = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S) ||
                      IsKeyDown(KEY_A) || IsKeyDown(KEY_D);

    float speedMultiplier = 1.0f;

    if (IsKeyDown(KEY_LEFT_SHIFT))
      speedMultiplier *= movMultiplier;

    auto [upChange, fwdChange, rgtChange] = getScaledDirectionVectors(dt * speedMultiplier);

    if (IsMouseButtonDown(1)) {
      DisableCursor();

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
    } else {
      EnableCursor();
    }
  }

  std::tuple<vec3, vec3, vec3> Camera::getScaledDirectionVectors(float dt) const {
    vec3 upChange  = worldUp * dt * movScale;
    vec3 fwdChange = localForward * dt * movScale;
    vec3 rgtChange = localRight * dt * movScale;

    return std::make_tuple(upChange, fwdChange, rgtChange);
  }

  glm::mat4 Camera::getViewMatrix() {
    // glm::mat4 M = glm::mat4(1);
    // M           = glm::translate(M, lookFrom.toGlm());
    // M           = M * glm::eulerAngleXYZ(angle.x, angle.y, angle.z) ;

    // glm::vec3 eye    = M * glm::vec4(0, 0, 0, 1);
    // glm::vec3 center = M * glm::vec4(0, 0, -1, 1);
    // glm::vec3 up     = M * glm::vec4(0, 1, 0, 0);

    return glm::lookAt(lookFrom.toGlm(), lookAt.toGlm(), worldUp.toGlm());
  }

  glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(glm::radians(vFov), aspectRatio, 0.01f, 1000.0f);
  }
} // namespace rt