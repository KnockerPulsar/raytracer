#include "Camera.h"
#include "Ray.h"

namespace raytracer {
  Ray Camera::GetRay(float u, float v) const {
    return Ray(origin,
               lower_left_corner + horizontal * u + vertical * v - origin);
  }
} // namespace raytracer