#include "Camera.h"
#include "Ray.h"

namespace raytracer {
  Ray Camera::GetRay(float s, float t) const {
    Vec3 rd = lensRadius * Vec3::RandomInUnitDisc();
    Vec3 offset = u*rd.x + v*rd.y;
    return Ray(lookFrom + offset,
               lower_left_corner + horizontal * s + vertical * t - lookFrom -offset);
  }
} // namespace raytracer