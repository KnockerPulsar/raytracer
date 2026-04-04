#include "Box.h"

#include "common/raycast.h"

namespace rt::raytracer {
  Box::Box(const vec3 &extents, Transformation const &transformation, sPtr<raytracer::Material> const &material)
      : Raytracable(transformation, material, AABB{-extents/2, extents/2}), extents{extents} {}

  Box::Box(float edgeLength, Transformation const &transformation, sPtr<raytracer::Material> const &material)
      : Box(vec3(edgeLength), transformation, material) {}

  bool Box::Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const {
    // Bounding box is in worldspace as an optimization
    if (!worldspaceBoundingBox.Hit(worldspaceRay, t_min, t_max))
      return {};

    auto const raycastResult = raycastInLocalspace(
        worldspaceRay, transformation, [&](auto const &localspaceRay) -> std::optional<rt::LocalspaceRaycastResult> {
          // We raycast in localspace because that's faster than creating an oriented bounding box.
          return rt::raycastAABBLocalspace(localspaceRay, -extents / 2, extents / 2, t_min, t_max);
        });

    if (!raycastResult)
      return false;

    auto const [worldspaceNormal, t] = *raycastResult;

    rec.p = worldspaceRay.At(t);
    rec.normal = worldspaceNormal;
    rec.t = t;
    // TODO UVs
    rec.mat_ptr    = material;
    return true;
  }
} // namespace rt
