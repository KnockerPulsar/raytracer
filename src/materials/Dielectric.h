#pragma once
#include "../Defs.h"
#include "Material.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include "../data_structures/Vec3.h"
#include <memory>

namespace raytracer {
  class Dielectric : public Material {
  public:
    sPtr<Texture> albedo;
    float         refractionIndex;

    Dielectric(float refIdx, Vec3 color = Vec3(1.0))
        : refractionIndex(refIdx), albedo(std::make_shared<SolidColor>(color)) {
    }

    Dielectric(float refIdx, sPtr<Texture> tex)
        : refractionIndex(refIdx), albedo(tex) {}

    Dielectric(json materialJson) {
      refractionIndex = materialJson["refraction_index"].get<float>();
      albedo          = TextureFactory::FromJson(materialJson["texture"]);
    }

    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const override {

      attenuation   = albedo->Value(rec.u, rec.v, rec.p);
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

} // namespace raytracer