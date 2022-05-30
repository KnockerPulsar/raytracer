#pragma once
#include "../Defs.h"
#include "../Hittable.h"
#include "../textures/TextureFactory.h"
#include "Material.h"

using nlohmann::json;

namespace rt {

  /**
   * @brief Contains logic to model lambertian/diffuse materials
   */
  class Lambertian : public Material {
  public:
    sPtr<Texture> albedo; // Base matte/diffuse texture

    Lambertian() = default;

    Lambertian(const vec3 &color) : albedo(ms<SolidColor>(color)) {}

    Lambertian(sPtr<Texture> color) : albedo(color) {}

    Lambertian(const sPtr<Lambertian> &l) : albedo(l->albedo) {}

    /**
     * @brief Scattering logic, scatters ray in random direction.
     *
     * @param r_in
     * @param rec
     * @param attenuation
     * @param scattered
     * @return true
     * @return false
     */
    virtual bool scatter(const Ray &rIn, HitRecord &rec, vec3 &attenuation, Ray &scattered) const override {

      vec3 scatterDir = rec.normal + vec3::RandomUnitVec();

      if (scatterDir.NearZero())
        scatterDir = rec.normal;

      scattered   = Ray(rec.p, scatterDir, rIn.time);
      attenuation = albedo->Value(rec.u, rec.v, rec.p);
      return true;
    }

    json GetJson() const override { return json{{"type", "lambertian"}, {"texture", albedo->GetJson()}}; }

    virtual void OnImgui() override {
      albedo->OnImgui();
    }
  };

  inline void from_json(const json &j, Lambertian &l) { l.albedo = TextureFactory::FromJson(j["texture"]); }

  inline void to_json(json &j, const Lambertian &l) { j = l.GetJson(); }
} // namespace rt
