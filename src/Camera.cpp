#include "Camera.h"
#include "Defs.h"
#include "Hittable.h"
#include "Ray.h"
#include "Util.h"
#include <raylib.h>

namespace rt {
  Ray Camera::GetRay(float s, float t) const {
    vec3 rd     = lensRadius * vec3::RandomInUnitDisc();
    vec3 offset = u * rd.x + v * rd.y;
    return Ray(lookFrom + offset,
               lower_left_corner + horizontal * s + vertical * t - lookFrom - offset,
               RandomFloat(time0, time1));
  }

  void Camera::Rasterize(std::vector<sPtr<Hittable>> rasterizables) {
    ClearBackground(BLACK);
    BeginDrawing();
    BeginMode3D(
        Camera3D{.position = lookFrom, .target = lookAt, .up = vUp, .fovy = vFov, .projection = CAMERA_PERSPECTIVE});
    for (auto &&raster : rasterizables) {
      raster->RasterizeTransformed(raster->transformation);
    }
    EndMode3D();
    EndDrawing();
  }
} // namespace rt