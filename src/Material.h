#pragma once

#include "Hittable.h"
#include "raytracer.h"

namespace raytracer {

struct HitRecord;

class Material {
 public:
  virtual bool scatter(const Ray& r_in,
                       HitRecord& rec,
                       vec3& attenuation,
                       Ray& scattered) const = 0;
};

class Lambertian : public Material {
 private:
  vec3 albedo;

 public:
  Lambertian(const vec3 color) : albedo(color) {}

  virtual bool scatter(const Ray& r_in,
                       HitRecord& rec,
                       vec3& attenuation,
                       Ray& scattered) const override {
    vec3 scatter_dir = rec.normal + random_unit_vec();

    if (vec3_near_zero(scatter_dir))
      scatter_dir = rec.normal;

    scattered = Ray(rec.p, scatter_dir);
    attenuation = albedo;
    return true;
  }
};

class Metal : public Material {
 public:
  vec3 albedo;
  float fuzz;

  Metal(const vec3& color, float f) : albedo(color), fuzz(f < 1 ? f : 1) {}

  bool scatter(const Ray& r_in,
               HitRecord& rec,
               vec3& attenuation,
               Ray& scattered) const override {
    vec3 in_normalized = r_in.direction.Normalize();
    vec3 reflected = vec3_reflect(in_normalized, rec.normal);
    scattered = Ray(rec.p, reflected + random_in_unit_sphere() * fuzz);
    attenuation = albedo;
    return (scattered.direction.DotProduct(rec.normal) > 0);
  }
};

}  // namespace raytracer
