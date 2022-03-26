#include "Camera.h"
#include "Ray.h"
#include "Util.h"

namespace rt {
  Ray Camera::GetRay(float s, float t) const {
    vec3 rd     = lensRadius * vec3::RandomInUnitDisc();
    vec3 offset = u * rd.x + v * rd.y;
    return Ray(lookFrom + offset,
               lower_left_corner + horizontal * s + vertical * t - lookFrom -
                   offset,
               RandomFloat(time0, time1));
  }
} // namespace rt