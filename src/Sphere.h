#pragma once

#include "AABB.h"
#include "Hittable.h"
#include "MaterialFactory.h"
#include "Ray.h"
#include "Vec3.h"
#include <cmath>
#include <raylib.h>
#include "Material.h"

namespace raytracer {

  class Sphere : public Hittable {
  public:
    float                radius;
    Vec3                 center;
    shared_ptr<Material> mat_ptr;

    Sphere(float r, Vec3 pos, shared_ptr<Material> m)
        : radius(r), center(pos), mat_ptr(m) {}

    Sphere(nlohmann::json sphereJson){
      center = Vec3::FromJson(sphereJson["pos"]);
      radius = sphereJson["radius"].get<float>();
      mat_ptr = MaterialFactory::FromJson(sphereJson["material"]);
    }

    bool Hit(const Ray &r, float t_min, float t_max,
             HitRecord &rec) const override;
    bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

  private:
    static void GetSphereUV(const Vec3 &p, float &u, float &v) {
      float theta = std::acos(-p.y);
      float phi   = std::atan2(-p.z, p.x) + PI;

      v = theta / PI;
      u = phi / (2 * PI);
    }
  };

  bool Sphere::Hit(const Ray &r, float t_min, float t_max,
                   HitRecord &rec) const {
    // Vector between ray origin and sphere center
    Vec3  oc     = r.origin - this->center;
    float a      = r.direction.SqrLen();
    float half_b = Vec3::DotProd(oc, r.direction);
    float c      = oc.Len() * oc.Len() - radius * radius;

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

    rec.t                     = root;
    rec.p                     = r.At(rec.t);
    const Vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    GetSphereUV(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
  }

  bool Sphere::BoundingBox(float t0, float t1, AABB &outputBox) const {
    outputBox = AABB(center - Vec3(radius), center + Vec3(radius));

    return true;
  }

} // namespace raytracer