#pragma once

#include "AABB.h"
#include "Ray.h"
#include <memory>
using std::shared_ptr;

namespace raytracer {
  class Material;

  struct HitRecord {
    Vec3                 p;
    Vec3                 normal;
    shared_ptr<Material> mat_ptr;
    float                t, u, v;
    bool                 front_face;

    inline void set_face_normal(const Ray &r, const Vec3 &outward_normal) {
      front_face = Vector3DotProduct(outward_normal, r.direction) < 0;
      normal     = front_face ? outward_normal : outward_normal * -1;
    }
  };

  class Hittable {
  public:
    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const = 0;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const = 0;
  };
} // namespace raytracer
