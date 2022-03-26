#pragma once
#include "../Defs.h"
#include "../Hittable.h"
#include "../textures/TextureFactory.h"
#include "Material.h"

using nlohmann::json;

namespace rt {

  class Lambertian : public Material {
  public:
    sPtr<Texture> albedo;

    Lambertian() = default;

    Lambertian(const vec3 &color)
        : albedo(std::make_shared<SolidColor>(color)) {}

    Lambertian(sPtr<Texture> color) : albedo(color) {}

    Lambertian(const sPtr<Lambertian> &l) : albedo(l->albedo) {}

    // Lambertian(json materialJson) {
    //   albedo = TextureFactory::FromJson(materialJson["texture"]);
    // }

    virtual bool scatter(const Ray &r_in, HitRecord &rec, vec3 &attenuation,
                         Ray &scattered) const override {

      vec3 scatter_dir = rec.normal + vec3::RandomUnitVec();

      if (scatter_dir.NearZero())
        scatter_dir = rec.normal;

      scattered   = Ray(rec.p, scatter_dir, r_in.time);
      attenuation = albedo->Value(rec.u, rec.v, rec.p);
      return true;
    }

    json GetJson() const override {
      return json{{"type", "lambertian"}, {"texture", albedo->GetJson()}};
    }
  };

  inline void from_json(const json &j, Lambertian &l) {
    l.albedo = TextureFactory::FromJson(j["texture"]);
  }

  inline void to_json(json &j, const Lambertian &l) { j = l.GetJson(); }
} // namespace rt
