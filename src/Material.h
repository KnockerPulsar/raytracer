#pragma once

#include "Hittable.h"
#include "SolidColor.h"
#include "Texture.h"
#include <memory>

namespace raytracer {

  struct HitRecord;

  class Material {
  public:
    virtual Vec3 emitted(float u, float v, const Vec3 &p) const {
      return Vec3::Zero();
    }
    
    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const = 0;
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

  class DiffuseLight : public Material {
  public:
    shared_ptr<Texture> emit;

    DiffuseLight(shared_ptr<Texture> a) : emit(a) {}
    DiffuseLight(Vec3 c) : emit(std::make_shared<SolidColor>(c)) {}

    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const override {
      return false;
    }

    virtual Vec3 emitted(float u, float v, const Vec3 &p) const override {
      return emit->Value(u, v, p);
    }
  };
}; // namespace raytracer
