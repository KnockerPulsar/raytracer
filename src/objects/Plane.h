#pragma once
#include "common/Hittable.h"
#include "common/raycast.h"

#include <raylib.h>

namespace rt::raytracer {
  // Base case is an XZ plane
  class Plane : public rt::Hittable {
  public:
    Plane(float width, float height, Transformation const &transformation, sPtr<raytracer::Material> const &material)
        : Hittable(transformation, material,
                   AABB{vec3{-width / 2, -constants::epsilon, -height / 2},
                        vec3{width / 2, constants::epsilon, height / 2}}),
          width(width), height(height) {}

    virtual bool Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const override {
      // TODO can this be done in a less hacky way?
      auto x = 0;
      auto z = 0;

      auto const raycastResult = rt::raycastInLocalspace(
          worldspaceRay, transformation, [&](auto const &localspaceRay) -> std::optional<LocalspaceRaycastResult> {
            if (!boundingBox.Hit(localspaceRay, t_min, t_max))
              return {};

            return rt::raycastPlane(localspaceRay, width, height, t_min, t_max)
                .transform([&](auto const planeRaycastResult) {
                  auto const hitpoint = localspaceRay.At(planeRaycastResult.t);
                  x                   = hitpoint.x;
                  z                   = hitpoint.z;
                  auto const normal   = vec3{0, 1, 0};
                  return LocalspaceRaycastResult{hitpoint, normal, planeRaycastResult.t};
                });
          });

      if (!raycastResult)
        return false;

      auto const [worldspaceHitPoint, worldspaceNormal, t] = *raycastResult;

      auto const x0 = -width / 2;
      auto const x1 = width / 2;
      auto const z0 = -height / 2;
      auto const z1 = height / 2;


      rec.p = worldspaceHitPoint;
      rec.setFaceNormal(worldspaceRay, worldspaceNormal);
      rec.u = (x - x1) / (x - x1);
      rec.v = (z - z0) / (z - z1);
      rec.t = t;
      rec.mat_ptr    = material;
      return true;
    }

  private:
    float const width, height;
  };
} // namespace rt
