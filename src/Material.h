#pragma once

#include "Hittable.h"

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

class Dielectric : public Material {
 public:
  float refractionIndex;

  Dielectric(float refIdx) : refractionIndex(refIdx) {}
  virtual bool scatter(const Ray& r_in,
                       HitRecord& rec,
                       Vec3& attenuation,
                       Ray& scattered) const override {
    attenuation = Vec3(1.0);
    float refraction_ratio =
        rec.front_face ? (1 / refractionIndex) : refractionIndex;

    Vec3 unit_dir = r_in.direction.Normalize();
    float cos_theta = fmin(Vec3::DotProd(-unit_dir, rec.normal), 1.0);
    float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = refraction_ratio*sin_theta > 1.0;
    Vec3 dir;

    if(cannot_refract)
      dir = unit_dir.Reflect(rec.normal);
    else
      dir = unit_dir.Refract(rec.normal, refraction_ratio);

    scattered = Ray(rec.p, dir);
    return true;
  }
};
};  // namespace raytracer
