#include "AARect.h"

namespace rt::raytracer {
  XYRect::XYRect(float _x0, float _x1, float _y0, float _y1, float _z) : x0(_x0), x1(_x1), y0(_y0), y1(_y1), z(_z) {}

  bool XYRect::Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
    float t = (z - r.origin.z) / r.direction.z;

    if (t < t_min || t > t_max)
      return false;

    auto [x, y, _] = r.At(t);

    if (x < x0 || x > x1 || y < y0 || y > y1)
      return false;

    rec.u = (x - x0) / (x - x1);
    rec.v = (y - y0) / (y - y1);
    rec.t = t;

    vec3 outwardNormal = vec3(0, 0, 1);
    rec.set_face_normal(r, outwardNormal);
    rec.mat_ptr    = material;
    rec.p          = vec3(x, y, z);
    rec.closestHit = (Hittable *)this;
    return true;
  }

  bool XYRect::BoundingBox(float t0, float t1, AABB &outputBox) const {
    // Add some padding to the thin dimension to prevent issues
    outputBox = AABB(vec3(x0, y0, z - 1e-4), vec3(x1, y1, z + 1e-4));
    return true;
  }

  XZRect::XZRect(float _x0, float _x1, float _z0, float _z1, float _y) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), y(_y) {}

  bool XZRect::Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
    float t = (y - r.origin.y) / r.direction.y;

    if (t < t_min || t > t_max)
      return false;

    auto [x, _, z] = r.At(t);

    if (x < x0 || x > x1 || z < z0 || z > z1)
      return false;

    rec.u = (x - x0) / (x - x1);
    rec.v = (z - z0) / (z - z1);
    rec.t = t;

    vec3 outwardNormal = vec3(0, 1, 0);
    rec.set_face_normal(r, outwardNormal);
    rec.mat_ptr    = material;
    rec.p          = vec3(x, y, z);
    rec.closestHit = (Hittable *)this;
    return true;
  }

  bool XZRect::BoundingBox(float t0, float t1, AABB &outputBox) const {
    // Add some padding to the thin dimension to prevent issues
    outputBox = AABB(vec3(x0, y - 1e-4, z0), vec3(x1, y + 1e-4, z1));
    return true;
  }

  YZRect::YZRect(float _y0, float _y1, float _z0, float _z1, float _x) : y0(_y0), y1(_y1), z0(_z0), z1(_z1), x(_x) {}

  bool YZRect::Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
    float t = (x - r.origin.x) / r.direction.x;

    if (t < t_min || t > t_max)
      return false;

    auto [_, y, z] = r.At(t);

    if (y < y0 || y > y1 || z < z0 || z > z1)
      return false;

    rec.u = (y - y0) / (y - y1);
    rec.v = (z - z0) / (z - z1);
    rec.t = t;

    vec3 outwardNormal = vec3(1, 0, 0);
    rec.set_face_normal(r, outwardNormal);
    rec.mat_ptr    = material;
    rec.p          = vec3(x, y, z);
    rec.closestHit = (Hittable *)this;
    return true;
  }

  bool YZRect::BoundingBox(float t0, float t1, AABB &outputBox) const {
    // Add some padding to the thin dimension to prevent issues
    outputBox = AABB(vec3(x - 1e-4, y0, z0), vec3(x + 1e-4, y1, z1));
    return true;
  }
} // namespace rt::raytracer
