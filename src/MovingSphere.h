#pragma once
#include "Hittable.h"
#include "Material.h"
#include "Vec3.h"
#include <memory>

namespace raytracer {
  class MovingSphere : public raytracer::Hittable {

  public:
    Vec3                 center0, center1;
    float                time0, time1;
    float                radius;
    shared_ptr<Material> mat_ptr;

    MovingSphere() = default;
    MovingSphere(Vec3 c0, Vec3 c1, float t0, float t1, float r,
                 shared_ptr<Material> m)
        : center0(c0), center1(c1), time0(t0), time1(t1), radius(r),
          mat_ptr(m) {}

    virtual bool Hit(const Ray &r, float tMin, float tMax,
                     HitRecord &rec) const override {
      // Vector between ray origin and sphere center
      Vec3  oc     = r.origin - this->CurrCenter(r.time);
      float a      = r.direction.SqrLen();
      float half_b = Vec3::DotProd(oc, r.direction);
      float c      = oc.Len() * oc.Len() - radius * radius;

      float discriminant = half_b * half_b - a * c;
      if (discriminant < 0)
        return false;

      float sqrt_disc = sqrt(discriminant);

      float root = (-half_b - sqrt_disc) / a;
      if (root < tMin || tMax < root) {
        root = (-half_b + sqrt_disc) / a;
        if (root < tMin || tMax < root)
          return false;
      }

      rec.t                     = root;
      rec.p                     = r.At(rec.t);
      const Vec3 outward_normal = (rec.p - CurrCenter(r.time)) / radius;
      rec.set_face_normal(r, outward_normal);
      rec.mat_ptr = mat_ptr;

      return true;
    }

    Vec3 CurrCenter(float time) const;
  };

  Vec3 MovingSphere::CurrCenter(float time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
  }
} // namespace raytracer