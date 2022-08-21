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

    Lambertian() = default;

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

    json toJson() const override { return json{{"type", "lambertian"}, {"texture", albedo->toJson()}}; }

    virtual void OnDerivedImgui() override {
    }
  };

  inline void from_json(const json &j, Lambertian &l) { l.albedo = TextureFactory::FromJson(j["texture"]); }

  inline void to_json(json &j, const Lambertian &l) { j = l.toJson(); }
} // namespace rt
