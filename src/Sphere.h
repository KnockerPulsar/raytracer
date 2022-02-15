#pragma once

#include "../include/raylib-cpp/include/raylib-cpp.hpp"
#include "Hittable.h"
#include "Ray.h"
#include <raylib.h>

namespace raytracer {

class Sphere : public Hittable {

public:
  float radius;
  raylib::Vector3 center;
  shared_ptr<Material> mat_ptr;

  Sphere(float r, raylib::Vector3 pos, shared_ptr<Material> m)
      : radius(r), center(pos), mat_ptr(m) {}

  bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const;
};

bool Sphere::Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {

  // Vector between ray origin and sphere center
  raylib::Vector3 oc = r.Origin() - this->center;
  float a = r.direction.Length() * r.direction.Length();
  float half_b = oc.DotProduct(r.direction);
  float c = oc.Length() * oc.Length() - radius * radius;

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
  const raylib::Vector3 outward_normal = (rec.p - center) / radius;
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = mat_ptr;

  return true;
}

} // namespace raytracer