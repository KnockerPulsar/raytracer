#include "Camera.h"

#include "Util.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>
#include <raylib.h>
#include <raymath.h>

#include <cmath>

namespace rt {

  Camera::Camera(vec3 lookFrom, vec3 lookAt, vec3 vUp, float vFov, float aspectRatio, float aperature, float focusDist,
                 float time0, float time1)
      : lookFrom(lookFrom), lookAt(lookAt), worldUp(vUp), vFov(vFov), aspectRatio(aspectRatio), focusDist(focusDist),
        time0(time0), time1(time1), lensRadius(aperature / 2) {
    // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
    float theta = DegressToRadians(vFov);

    // https://raytracing.github.io/images/fig-1.14-cam-view-geom.jpg
    float      h           = tan(theta / 2);
    auto const frameHeight = 2.0 * h;
    auto const frameWidth  = aspectRatio * frameHeight;

    // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
    localForward = (lookAt - lookFrom).Normalize();
    localRight   = -vec3::CrsProd(worldUp, localForward).Normalize();
    localUp      = -vec3::CrsProd(localForward, localRight);

    horizontal      = focusDist * frameWidth * localRight;
    vertical        = focusDist * frameHeight * localUp;
    lowerLeftCorner = this->lookFrom - horizontal / 2 - vertical / 2 + focusDist * localForward;
  }

rt::Ray Camera::GetRay(float s, float t) const {
  vec3 rd     = lensRadius * vec3::RandomInUnitDisc();
  vec3 offset = localRight * rd.x + localUp * rd.y;
  return rt::Ray(lookFrom + offset, (lowerLeftCorner + horizontal * s + vertical * t - lookFrom - offset).Normalize(),
                 RandomFloat(time0, time1));
}
} // namespace rt
