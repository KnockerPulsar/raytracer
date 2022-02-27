#pragma once
#include "Ray.h"
#include "Vec3.h"

namespace raytracer {
  class Camera {
  public:
    Camera() = default;
    Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 vUp,
           float vFov, // vertical field-of-view in degrees
           float aspectRatio, float aperature, float focusDist) {

      float theta = DegressToRadians(vFov);

      // https://raytracing.github.io/images/fig-1.14-cam-view-geom.jpg
      float h               = tan(theta / 2);
      float viewport_height = 2.0 * h;
      float viewport_width  = aspectRatio * viewport_height;

      // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
      w = (lookFrom - lookAt).Normalize();
      u = Vec3::CrsProd(vUp, w).Normalize();
      v = Vec3::CrsProd(w, u);

      origin     = lookFrom;
      horizontal = focusDist * viewport_width * u;
      vertical   = focusDist * viewport_height * v;
      lower_left_corner =
          origin - horizontal / 2 - vertical / 2 - focusDist * w;

      lensRadius = aperature / 2;
    }

    raytracer::Ray GetRay(float s, float t) const;

  private:
    Vec3  origin, lower_left_corner, horizontal, vertical;
    Vec3  u, v, w;
    float lensRadius;
  };
} // namespace raytracer
