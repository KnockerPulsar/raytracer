#pragma once
#include "../Defs.h"
#include "../data_structures/vec3.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include "Material.h"
#include <memory>

namespace rt {
  class Dielectric : public Material {
  public:
    sPtr<Texture> albedo;
    float         refractionIndex;

    Dielectric() = default;

    Dielectric(float refIdx, vec3 color = vec3(1.0))
        : refractionIndex(refIdx), albedo(std::make_shared<SolidColor>(color)) {
    }

    Dielectric(float refIdx, sPtr<Texture> tex)
        : refractionIndex(refIdx), albedo(tex) {}

    virtual bool scatter(const Ray &r_in, HitRecord &rec, vec3 &attenuation,
                         Ray &scattered) const override {

      attenuation   = albedo->Value(rec.u, rec.v, rec.p);
      float ref_idx = rec.front_face ? (1 / refractionIndex) : refractionIndex;

      vec3  unit_dir  = r_in.direction.Normalize();
      float cos_theta = fmin(vec3::DotProd(-unit_dir, rec.normal), 1.0);
      float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

      bool  cannot_refract = ref_idx * sin_theta > 1.0;
      float reflectance    = Reflectance(cos_theta, ref_idx);
      vec3  dir;

      // Either reflect when the refraction index and sin_theta are large
      // enought Or just reflect randomly. Otherwise, refract.
      if (cannot_refract || reflectance > RandomFloat())
        dir = unit_dir.Reflect(rec.normal);
      else
        dir = unit_dir.Refract(rec.normal, ref_idx);

      scattered = Ray(rec.p, dir, r_in.time);
      return true;
    }

    virtual json GetJson() const override {
      return json{{"type", "dielectri"},
                  {"refraction_index", refractionIndex},
                  {"texture", albedo->GetJson()}};
    }

  private:
    static float Reflectance(float cos, float ref_idx) {
      // Schlick's approx.
      float r0 = (1 - ref_idx) / (1 + ref_idx);
      r0       = r0 * r0;
      return r0 + (1 - r0) * pow(1 - cos, 5);
    }
  };

  inline void from_json(const json &j, Dielectric &d) {
    d.refractionIndex = j["refraction_index"].get<float>();
    d.albedo          = TextureFactory::FromJson(j["texture"]);
  }

  inline void to_json(json &j, const Dielectric &d) { j = d.GetJson(); }

} // namespace rt