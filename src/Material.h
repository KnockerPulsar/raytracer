#pragma once

#include "Hittable.h"
#include "raytracer.h"

namespace raytracer {

struct HitRecord;

class Material {
 public:
  virtual bool scatter(const Ray& r_in,
                       HitRecord& rec,
                       Vec3& attenuation,
                       Ray& scattered) const = 0;
};

class Lambertian : public Material {
 private:
  Vec3 albedo;

 public:
  Lambertian(const Vec3 color) : albedo(color) {}

  virtual bool scatter(const Ray& r_in,
                       HitRecord& rec,
                       Vec3& attenuation,
                       Ray& scattered) const override {
    Vec3 scatter_dir = rec.normal + Vec3::RandomUnitVec();

    if (scatter_dir.NearZero())
      scatter_dir = rec.normal;

    scattered = Ray(rec.p, scatter_dir);
    attenuation = albedo;
    return true;
  }
};

class Metal : public Material {
 public:
  Vec3 albedo;
  float fuzz;

  Metal(const Vec3& color, float f) : albedo(color), fuzz(f < 1 ? f : 1) {}

  bool scatter(const Ray& r_in,
               HitRecord& rec,
               Vec3& attenuation,
               Ray& scattered) const override {
    Vec3 in_normalized = r_in.direction.Normalize();
    Vec3 reflected = in_normalized.Reflect(rec.normal);
    scattered = Ray(rec.p, reflected + Vec3::RandomInUnitSphere() * fuzz);
    attenuation = albedo;
    return (Vec3::DotProd(scattered.direction, rec.normal) > 0);
  }
};

}  // namespace raytracer
