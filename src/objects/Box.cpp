#include "Box.h"

#include "common/raycast.h"


namespace rt::raytracer {
  Box::Box(const vec3 &extents, Transformation const &transformation, sPtr<raytracer::Material> const &material)
      : Hittable(transformation, material, AABB{-extents/2, extents/2}) {}

  Box::Box(float edgeLength, Transformation const &transformation, sPtr<raytracer::Material> const &material)
      : Box(vec3(edgeLength), transformation, material) {}

  bool Box::Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const {
    auto const raycastResult = rt::raycastInLocalspace(
        worldspaceRay, transformation, [&](auto const localspaceRay) -> std::optional<LocalspaceRaycastResult> {
          if (!boundingBox.Hit(localspaceRay, t_min, t_max))
            return {};

          return rt::raycastBox(localspaceRay, boundingBox.min, boundingBox.max, t_min, t_max)
              .transform([&](auto const &boxRaycastResult) {
                auto const localspaceHitPoint = localspaceRay.At(boxRaycastResult.t);
                auto const localspaceNormal   = boxRaycastResult.normal;
                return rt::LocalspaceRaycastResult{localspaceHitPoint, localspaceNormal, boxRaycastResult.t};
              });
        });

    if (!raycastResult)
      return false;

    auto const [worldspaceHitPoint, worldspaceNormal, t] = *raycastResult;

    rec.p = worldspaceHitPoint;
    rec.normal = worldspaceNormal;
    rec.t = t;
    // TODO UVs
    rec.mat_ptr    = material;
    return true;
  }
} // namespace rt
