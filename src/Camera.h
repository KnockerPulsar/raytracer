#pragma once
#include "raytracer.h"

namespace raytracer {
class Camera {
public:
  Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 vUp,
         float vFov, // vertical field-of-view in degrees
         float aspectRatio) {

    float theta = DegressToRadians(vFov);

    // https://raytracing.github.io/images/fig-1.14-cam-view-geom.jpg
    float h               = tan(theta / 2);
    float viewport_height = 2.0 * h;
    float viewport_width  = aspectRatio * viewport_height;

    // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
    Vec3 w = (lookFrom - lookAt).Normalize();
    Vec3 u = Vec3::CrsProd(vUp, w).Normalize();
    Vec3 v = Vec3::CrsProd(w, u);

    origin            = lookFrom;
    horizontal        = viewport_width * u;
    vertical          = viewport_height * v;
    lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
  }

  Ray GetRay(float u, float v) {
    return Ray(origin,
               lower_left_corner + horizontal * u + vertical * v - origin);
  }

private:
  Vec3 origin, lower_left_corner, horizontal, vertical;
};
} // namespace raytracer
