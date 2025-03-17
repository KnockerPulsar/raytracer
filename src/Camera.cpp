#include "Camera.h"

#include "rt.h"
#include "Util.h"

#include <imgui.h>
#include <raylib.h>
#include <raymath.h>
#include <../vendor/glm/glm/ext/matrix_clip_space.hpp>
#include <../vendor/glm/glm/ext/matrix_transform.hpp>

#include <cmath>

namespace rt {

Camera::Camera(vec3 lookFrom, vec3 lookAt, vec3 vUp, float vFov,
               float aspectRatio, float aperature, float focusDist, float time0,
               float time1)
    : lookFrom(lookFrom), lookAt(lookAt), worldUp(vUp), vFov(vFov),
      aspectRatio(aspectRatio), aperature(aperature), focusDist(focusDist),
      time0(time0), time1(time1), lensRadius(aperature / 2) {
  // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
  float theta = DegressToRadians(vFov);

  // https://raytracing.github.io/images/fig-1.14-cam-view-geom.jpg
  // TODO fix fov when using a frame size different from the editor size
  // the output image should only show the part inside the frame preview
  // rectangle
  float h = tan(theta / 2);
  auto const frameHeight = 2.0 * h;
  auto const frameWidth = aspectRatio * frameHeight;

  // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
  RecomputeLocalBases();
  horizontal      = focusDist * frameWidth * localRight;
  vertical        = focusDist * frameHeight * localUp;
  lowerLeftCorner = this->lookFrom - horizontal / 2 - vertical / 2 + focusDist * localForward;
}

  // Should probably be moved to from_json()?
  Camera::Camera(nlohmann::json cameraJson, float aspectRatio)
      : Camera(
            cameraJson["look_from"].get<vec3>(),
            cameraJson["look_at"].get<vec3>(),
            cameraJson["v_up"].get<vec3>(),
            cameraJson["fov"].get<float>(),
            aspectRatio,
            cameraJson["aperature"].get<float>(),
            cameraJson["focus_dist"].get<float>(),
            cameraJson["time0"].get<float>(),
            cameraJson["time1"].get<float>()
        ) {
  }

  // Used for raytracing
  rt::Ray Camera::GetRay(float s, float t) const {
    vec3 rd     = lensRadius * vec3::RandomInUnitDisc();
    vec3 offset = localRight * rd.x + localUp * rd.y;
    return rt::Ray(
        lookFrom + offset,
        (lowerLeftCorner + horizontal * s + vertical * t - lookFrom - offset).Normalize(),
        RandomFloat(time0, time1)
    );
  }


  void Camera::RenderImgui() {
    if (ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize)) {

      ImGui::DragFloat("Vertical FOV", &vFov, 0.1f, 20, 180);

      ImGui::DragFloat("Focus distance", &focusDist, 0.01, 0.1, 1000);

      ImGui::DragFloat("Aperature", &aperature, 0.01, 10e-6, 10);
    }
    ImGui::End();
  }

  std::tuple<vec3, vec3, vec3>
  Camera::getScaledDirectionVectors(float dt) const {
    vec3 upChange = worldUp * dt;
    vec3 fwdChange = localForward * dt;
    vec3 rgtChange = localRight * dt;

    return std::make_tuple(upChange, fwdChange, rgtChange);
  }

  void Camera::RecomputeLocalBases() {
    localForward = (lookAt - lookFrom).Normalize();                   // back vector
    localRight   = -vec3::CrsProd(worldUp, localForward).Normalize(); // Right vector
    localUp      = -vec3::CrsProd(localForward, localRight);          // Local up vector, vUp is the world up vector
  }
} // namespace rt
