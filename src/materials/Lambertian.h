#pragma once
#include "../Defs.h"
#include "../Hittable.h"
#include "../textures/TextureFactory.h"
#include "Constants.h"
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
    virtual bool scatter(const Ray &rIn, HitRecord &rec, vec3 &alb, Ray &scattered, float& pdf) const override{

      vec3 scatterDir = vec3::RandomInHemisphere(rec.normal);

      scattered   = Ray(rec.p, scatterDir.Normalize(), rIn.time);
      alb = albedo->Value(rec.u, rec.v, rec.p);
      pdf = 0.5f / pi;
      return true;
    }

    virtual float scatteringPdf(const Ray& rIn, const HitRecord& rec, const Ray& scattered) override{ 
      auto cosine = vec3::DotProd(rec.normal, scattered.direction.Normalize());
      return cosine < 0? epsilon : cosine / pi;
    }

    json toJson() const override { return json{{"type", "lambertian"}, {"texture", albedo->toJson()}}; }

    virtual void OnDerivedImgui() override {}
  };

  inline void from_json(const json &j, Lambertian &l) { l.albedo = TextureFactory::FromJson(j["texture"]); }

  inline void to_json(json &j, const Lambertian &l) { j = l.toJson(); }
} // namespace rt
