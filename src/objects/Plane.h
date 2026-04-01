#pragma once
#include "common/Hittable.h"
#include "common/raycast.h"

#include <raylib.h>

namespace rt::raytracer {
  // Base case is an XZ plane
  class Plane : public rt::Hittable {
  public:
    Plane() {}
    Plane(float width, float height)
        : width(width), height(height) {}

    virtual bool Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const override {
      // TODO can this be done in a less hacky way?
      auto x = 0;
      auto z = 0;
      auto const raycastResult = rt::raycastInLocalspace(worldspaceRay, transformation, [&](auto const &localspaceRay) {
        return rt::raycastPlane(localspaceRay, width, height, t_min, t_max)
            .transform([&](auto const planeRaycastResult) {
              auto const hitpoint = localspaceRay.At(planeRaycastResult.t);
              x = hitpoint.x;
              z = hitpoint.z;
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
      rec.set_face_normal(worldspaceRay, worldspaceNormal);
      rec.u = (x - width) / (x - x1);
      rec.v = (z - z0) / (z - z1);
      rec.t = t;
      rec.mat_ptr    = material;
      rec.closestHit = (Hittable *)this;
      return true;
    }

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override {
      // TODO
      // outputBox = transformation.regenAABB(AABB(
      //     std::vector<vec3>{this->t0.v0.p, this->t0.v1.p, this->t0.v2.p, this->t1.v0.p, this->t1.v1.p, this->t1.v2.p}));
      return true;
    }
  private:
    float width, height;
  };
} // namespace rt
