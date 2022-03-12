#pragma once

#include "AARect.h"
#include "HittableList.h"
#include "Ray.h"
#include <memory>

using std::make_shared;

namespace raytracer {
  class Box : public Hittable {
  public:
    Vec3         boxMin, boxMax;
    HittableList sides;

    Box() = default;
    Box(const Vec3 &p0, const Vec3 &p1, std::shared_ptr<Material> mat) {
      boxMin = p0;
      boxMax = p1;

      sides.Add(make_shared<XYRect>(p0.x, p1.x, p0.y, p1.y, p0.z, mat));
      sides.Add(make_shared<XYRect>(p0.x, p1.x, p0.y, p1.y, p1.z, mat));

      sides.Add(make_shared<XZRect>(p0.x, p1.x, p0.z, p1.z, p0.y, mat));
      sides.Add(make_shared<XZRect>(p0.x, p1.x, p0.z, p1.z, p1.y, mat));

      sides.Add(make_shared<YZRect>(p0.y, p1.y, p0.z, p1.z, p0.x, mat));
      sides.Add(make_shared<YZRect>(p0.y, p1.y, p0.z, p1.z, p1.x, mat));
    }

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override{
      outputBox = AABB(boxMin,boxMax);
      return true;
    }

    bool Hit(const Ray &r, float t_min, float t_max,
             HitRecord &rec) const override {
      return sides.Hit(r, t_min, t_max, rec);
    }
  };
} // namespace raytracer