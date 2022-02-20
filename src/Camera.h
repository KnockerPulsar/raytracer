#pragma once
#include "raytracer.h"

namespace raytracer {
class Camera {
 public:
  Camera() {
    float aspect_ratio = 16.0 / 9.0;
    float viewport_height = 2.0;
    float viewport_width = aspect_ratio * viewport_height;
    float focal_length = 1.0f;

    origin = Vec3::Zero();
    horizontal = Vec3(viewport_width, 0, 0);
    vertical = Vec3(0, viewport_height, 0);
    lower_left_corner =
        origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length);
  }

  Ray GetRay(float u, float v) {
    return Ray(origin,
               lower_left_corner + horizontal * u + vertical * v - origin);
  }

 private:
  Vec3 origin, lower_left_corner, horizontal, vertical;
};
}  // namespace raytracer
