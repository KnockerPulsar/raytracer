#pragma once
#include "../AABB.h"
#include "../Hittable.h"
#include "../data_structures/vec3.h"
#include "../materials/Material.h"
#include <memory>

namespace rt {
  class MovingSphere : public rt::Hittable {

  public:
    vec3  center0, center1;
    float time0, time1;
    float radius;

    MovingSphere() = default;
    MovingSphere(vec3 c0, vec3 c1, float t0, float t1, float r, shared_ptr<Material> m)
        : center0(c0), center1(c1), time0(t0), time1(t1), radius(r) {
      this->material = m;
    }

    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &rec) const override {
      // Vector between ray origin and sphere center
      vec3  oc     = r.origin - this->CurrCenter(r.time);
      float a      = r.direction.SqrLen();
      float half_b = vec3::DotProd(oc, r.direction);
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
      const vec3 outward_normal = (rec.p - CurrCenter(r.time)) / radius;
      rec.set_face_normal(r, outward_normal);
      rec.mat_ptr    = material;
      rec.closestHit = (Hittable *)this;
      return true;
    }

    vec3 CurrCenter(float time) const;
    bool BoundingBox(float t0, float t1, AABB &outputBox) const override;
  };

  vec3 MovingSphere::CurrCenter(float time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
  }

  bool MovingSphere::BoundingBox(float t0, float t1, AABB &outputBox) const {
    AABB box0 = AABB(CurrCenter(t0) - vec3(radius), CurrCenter(t0) + vec3(radius));

    AABB box1 = AABB(CurrCenter(t1) - vec3(radius), CurrCenter(t1) + vec3(radius));

    outputBox = AABB::SurroundingBox(box0, box1);
    return true;
  }

} // namespace rt