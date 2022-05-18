#include "Camera.h"
#include "../vendor/rlImGui/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"
#include "Util.h"
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <tuple>

#include "../vendor/glm/glm/gtx/fast_trigonometry.hpp"

namespace rt {
  Ray Camera::GetRay(float s, float t) const {
    vec3 rd     = lensRadius * vec3::RandomInUnitDisc();
    vec3 offset = u * rd.x + v * rd.y;
    return Ray(lookFrom + offset,
               lower_left_corner + horizontal * s + vertical * t - lookFrom - offset,
               RandomFloat(time0, time1));
  }

  void Camera::Rasterize(std::vector<sPtr<Hittable>> rasterizables) {
    BeginMode3D(
        Camera3D{.position = lookFrom, .target = lookAt, .up = vUp, .fovy = vFov, .projection = CAMERA_PERSPECTIVE});

    DrawGrid(10, 10);
    for (auto &&raster : rasterizables) {
      raster->RasterizeTransformed(raster->transformation);
    }
    EndMode3D();
  }

  void Camera::RaylibRotateCamera(Vector2 mousePositionDelta) {
    angle.x += (mousePositionDelta.y * -rotSensitity.y);
    angle.y += (mousePositionDelta.x * -rotSensitity.x);

    angle.x = glm::clamp(angle.x, xAngleClampMin * DEG2RAD, xAngleClampMax * DEG2RAD);
    angle.y = glm::wrapAngle(angle.y);

    glm::mat4 rotMat     = glm::eulerAngleYX(angle.y, angle.x);
    glm::vec4 defaultFwd = glm::vec4(0, 0, -1, 0);
    glm::vec4 defaultRgt = glm::vec4(1, 0, 0, 0);

    glm::vec3 rotatedFwd = rotMat * defaultFwd;
    glm::vec3 rotatedRgt = rotMat * defaultRgt;

    lookAt = lookFrom + rotatedFwd;
    rgt    = rotatedRgt;
    // vUp    = Vector3Normalize(Vector3CrossProduct(rgt, lookAt));
  }

  void Camera::RenderImgui(HittableList *objectList) {
    rlImGuiBegin();

    ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize);

    vec3 deltaPos = lookFrom;
    ImGui::DragFloat3("lookFrom", &lookFrom.x, 0.05);

    ImGui::Combo("Camera type", (int *)&controlType, controlTypeLabels, CameraControlType::controlTypesCount, 0);

    if (controlType == CameraControlType::lookAt)
      ImGui::DragFloat3("lookAt", &lookAt.x, 0.05);

    else {
      deltaPos -= lookFrom; // lookFrom = oldLookFrom + delta
      lookAt += (deltaPos);
      ImGui::DragFloat2("rotation", &angle.x, 0.05f);
      RaylibRotateCamera({0, 0});
    }

    ImGui::End();

    ImGui::Begin("Objects");
    for (auto &&o : objectList->objects) {
      o->onImmediateGui();
      ImGui::Separator();
    }
    ImGui::End();

    rlImGuiEnd();
  }

  void Camera::UpdateEditorCamera(float dt) {
    auto [upChange, fwdChange, rgtChange] = getScaledDirectionVectors(dt);
    bool keyPressed = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S) ||
                      IsKeyDown(KEY_A) || IsKeyDown(KEY_D);

    if (IsMouseButtonDown(1)) {
      DisableCursor();
      RaylibRotateCamera(GetMouseDelta());

      if (keyPressed) {
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

        RaylibRotateCamera(GetMouseDelta());
      }
    } else {
      EnableCursor();
    }
  }

  std::tuple<vec3, vec3, vec3> Camera::getScaledDirectionVectors(float dt) const {
    vec3 upChange  = vUp * dt * movScale;
    vec3 fwdChange = (lookAt - lookFrom).Normalize() * dt * movScale;
    vec3 rgtChange = rgt * dt * movScale;

    return std::make_tuple(upChange, fwdChange, rgtChange);
  }
} // namespace rt