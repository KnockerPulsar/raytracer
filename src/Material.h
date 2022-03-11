#pragma once

#include "Hittable.h"
#include "SolidColor.h"
#include "Texture.h"
#include <memory>

namespace raytracer {

  struct HitRecord;

  class Material {
  public:
    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const = 0;
  };

  class Lambertian : public Material {
  private:
    shared_ptr<Texture> albedo;

  public:
    Lambertian(const Vec3 &color)
        : albedo(std::make_shared<SolidColor>(color)) {}
    Lambertian(shared_ptr<Texture> color) : albedo(color) {}
    Lambertian(const shared_ptr<Lambertian>& l) { this->albedo = l->albedo; }

    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const override {

      Vec3 scatter_dir = rec.normal + Vec3::RandomUnitVec();

      if (scatter_dir.NearZero())
        scatter_dir = rec.normal;

      scattered   = Ray(rec.p, scatter_dir, r_in.time);
      attenuation = albedo->Value(rec.u, rec.v, rec.p);
      return true;
    }
  };

  class Metal : public Material {
  public:
    Vec3  albedo;
    float fuzz;

    Metal(const Vec3 &color, float f) : albedo(color), fuzz(f < 1 ? f : 1) {}

    bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                 Ray &scattered) const override {
      Vec3 in_normalized = r_in.direction.Normalize();
      Vec3 reflected     = in_normalized.Reflect(rec.normal);
      scattered =
          Ray(rec.p, reflected + Vec3::RandomInUnitSphere() * fuzz, r_in.time);
      attenuation = albedo;
      return (Vec3::DotProd(scattered.direction, rec.normal) > 0);
    }
  };

  class Dielectric : public Material {
  public:
    float refractionIndex;

    Dielectric(float refIdx) : refractionIndex(refIdx) {}

    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const override {

      attenuation   = Vec3(1.0);
      float ref_idx = rec.front_face ? (1 / refractionIndex) : refractionIndex;

      Vec3  unit_dir  = r_in.direction.Normalize();
      float cos_theta = fmin(Vec3::DotProd(-unit_dir, rec.normal), 1.0);
      float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

      bool  cannot_refract = ref_idx * sin_theta > 1.0;
      float reflectance    = Reflectance(cos_theta, ref_idx);
      Vec3  dir;

      // Either reflect when the refraction index and sin_theta are large
      // enought Or just reflect randomly. Otherwise, refract.
      if (cannot_refract || reflectance > RandomFloat())
        dir = unit_dir.Reflect(rec.normal);
      else
        dir = unit_dir.Refract(rec.normal, ref_idx);

      scattered = Ray(rec.p, dir, r_in.time);
      return true;
    }

  private:
    static float Reflectance(float cos, float ref_idx) {
      // Schlick's approx.
      float r0 = (1 - ref_idx) / (1 + ref_idx);
      r0       = r0 * r0;
      return r0 + (1 - r0) * pow(1 - cos, 5);
    }
  };
}; // namespace raytracer
