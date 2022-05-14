#include "Camera.h"
#include "../vendor/rlImGui/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"
#include "Util.h"
#include <raylib.h>
#include <raymath.h>

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
    for (auto &&raster : rasterizables) {
      raster->RasterizeTransformed(raster->transformation);
    }
    EndMode3D();
  }

  void Camera::RaylibRotateCamera(Vector2 mousePositionDelta) {
    angle.x += (mousePositionDelta.y * -rotSensitity.y);
    angle.y += (mousePositionDelta.x * -rotSensitity.x);

    // Angle clamp
    if (angle.x < xAngleClampMin * DEG2RAD)
      angle.x = xAngleClampMin * DEG2RAD;
    else if (angle.x > xAngleClampMax * DEG2RAD)
      angle.x = xAngleClampMax * DEG2RAD;

    glm::mat4 rotMat = glm::eulerAngleXYZ(angle.x, angle.y, angle.z);
    glm::vec4 defaultFwd = glm::vec4(0,0,-1, 1) ;

    glm::vec3 rotated = rotMat * defaultFwd;
    vec3 rot = {rotated.x, rotated.y, rotated.z};

    lookAt = rot;
    rgt    = Vector3Normalize(Vector3CrossProduct(lookAt, vUp));
    // vUp    = Vector3Normalize(Vector3CrossProduct(rgt, lookAt));
  }

  void Camera::RenderImgui(HittableList* objectList) {
    rlImGuiBegin();

    ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::DragFloat3("translation", &lookFrom.x, 0.05);
    ImGui::DragFloat3("look at", &lookAt.x, 0.05);
    if(ImGui::DragFloat3("rotation", &angle.x, 0.05f))
      RaylibRotateCamera({0,0});    

    ImGui::End();


    ImGui::Begin("Objects");
    for(auto&& o : objectList->objects){
      o->onImmediateGui();
      ImGui::Separator();
    }
    ImGui::End();

    rlImGuiEnd();
  }

  void Camera::UpdateEditorCamera() {

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      float dt = GetFrameTime();

      Vector3 elevationChange = Vector3Scale(vUp, dt * movScale);
      Vector3 fwdChange       = Vector3Scale(lookAt - lookFrom, dt * movScale);
      Vector3 rgtChange       = Vector3Scale(rgt, dt * movScale);

      bool keyPressed = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_W) || IsKeyDown(KEY_S) ||
                        IsKeyDown(KEY_A) || IsKeyDown(KEY_D);

      if (keyPressed) {
        if (IsKeyDown(KEY_SPACE)) {
          lookFrom = Vector3Add(lookFrom, elevationChange);
          lookAt   = Vector3Add(lookAt, elevationChange);
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
          lookFrom = Vector3Subtract(lookFrom, elevationChange);
          lookAt   = Vector3Subtract(lookAt, elevationChange);
        }
        if (IsKeyDown(KEY_W)) {
          lookFrom = Vector3Add(lookFrom, fwdChange);
          lookAt   = Vector3Add(lookAt, fwdChange);
        }
        if (IsKeyDown(KEY_S)) {
          lookFrom = Vector3Subtract(lookFrom, fwdChange);
          lookAt   = Vector3Subtract(lookAt, fwdChange);
        }
        if (IsKeyDown(KEY_A)) {
          lookFrom = Vector3Subtract(lookFrom, rgtChange);
          lookAt   = Vector3Subtract(lookAt, rgtChange);
        }
        if (IsKeyDown(KEY_D)) {
          lookFrom = Vector3Add(lookFrom, rgtChange);
          lookAt   = Vector3Add(lookAt, rgtChange);
        }
      }

      DisableCursor();
      RaylibRotateCamera(GetMouseDelta());
    } else
      EnableCursor();
  }
} // namespace rt