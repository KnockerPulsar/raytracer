#include "Box.h"

#include "common/raycast.h"


namespace rt::raytracer {
  Box::Box(const vec3 &extents, Transformation const &transformation, sPtr<raytracer::Material> const &material)
      : Hittable(transformation, material), extents(extents) {}

  Box::Box(float edgeLength, Transformation const &transformation, sPtr<raytracer::Material> const &material)
      : Box(vec3(edgeLength), transformation, material) {}

  bool Box::BoundingBox(float t0, float t1, AABB &outputBox) const {
    // outputBox = transformation.regenAABB(AABB(boxMin, boxMax));
    return true;
  }

  bool Box::Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const {
    auto const raycastResult = rt::raycastInLocalspace(worldspaceRay, transformation, [&](auto const localspaceRay) {
      return rt::raycastBox(localspaceRay, extents, t_min, t_max).transform([&](auto const &boxRaycastResult) {
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
    rec.closestHit = (Hittable *)this;
    return true;
  }
} // namespace rt
