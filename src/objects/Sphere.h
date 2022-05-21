#pragma once

#include "../AABB.h"
#include "../Hittable.h"
#include "../Ray.h"
#include "../data_structures/vec3.h"
#include "../materials/Material.h"
#include "../materials/MaterialFactory.h"
#include <cmath>
#include <raylib.h>
#include <raymath.h>

namespace rt {

  class Sphere : public Hittable {
  public:
    float                radius;
    vec3                 center;
    shared_ptr<Material> mat_ptr;


    Sphere() = default;
    Sphere(float r, vec3 pos, shared_ptr<Material> m)
        : radius(r), center(pos), mat_ptr(m) {}

    bool Hit(const Ray &r, float t_min, float t_max,
             HitRecord &rec) const override;

    bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    json GetJsonDerived() const override {
      return json{{"type", "sphere"},
                  {"pos", center},
                  {"radius", radius},
                  {"material", mat_ptr->GetJson()}};
    }
    
    void Rasterize() override {
      // RasterizeTransformed takes care of the transformation and rotation
      DrawSphere(Vector3Zero(), radius, RED);
    }

  private:
    static void GetSphereUV(const vec3 &p, float &u, float &v) {
      float theta = std::acos(-p.y);
      float phi   = std::atan2(-p.z, p.x) + PI;

      v = theta / PI;
      u = phi / (2 * PI);
    }
  };

  bool Sphere::Hit(const Ray &r, float t_min, float t_max,
                   HitRecord &rec) const {
    // Vector between ray origin and sphere center
    vec3  oc     = r.origin - this->center;
    float a      = r.direction.SqrLen();
    float half_b = vec3::DotProd(oc, r.direction);
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
    const vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    GetSphereUV(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;
    rec.closestHit = (Hittable*)this;

    return true;
  }

  bool Sphere::BoundingBox(float t0, float t1, AABB &outputBox) const {
    outputBox = AABB(center - vec3(radius), center + vec3(radius));

    return true;
  }

  inline void from_json(const json &j, Sphere &s) {
    s.center  = j["pos"].get<vec3>();
    s.radius  = j["radius"].get<float>();
    s.mat_ptr = MaterialFactory::FromJson(j["material"]);
    s.transformation = j["transform"].get<Transformation>();
  }

  inline void to_json(json &j, const Sphere &s) { j = s.GetJson(); }

} // namespace rt