#pragma once

#include <raylib.h>
#include "Hittable.h"
#include "Ray.h"
#include "Vec3.h"

namespace raytracer {

class Sphere : public Hittable {
 public:
  float radius;
  Vec3 center;
  shared_ptr<Material> mat_ptr;

  Sphere(float r, Vec3 pos, shared_ptr<Material> m)
      : radius(r), center(pos), mat_ptr(m) {}

  bool Hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const;
};

bool Sphere::Hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const {
  // Vector between ray origin and sphere center
  Vec3 oc = r.origin - this->center;
  float a = r.direction.SqrLen();
  float half_b = Vec3::DotProd(oc, r.direction);
  float c = oc.Len() * oc.Len() - radius * radius;

  float discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return false;

  float sqrt_disc = sqrt(discriminant);

  float root = (-half_b - sqrt_disc) / a;
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrt_disc) / a;
    if (root < t_min || t_max < root)
      return false;
  }

  rec.t = root;
  rec.p = r.At(rec.t);
  const Vec3 outward_normal = (rec.p - center) / radius;
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr;

  return true;
}

}  // namespace raytracer