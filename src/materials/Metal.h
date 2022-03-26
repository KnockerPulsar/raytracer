#pragma once
#include "../Defs.h"
#include "../materials/Material.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include <memory>

namespace rt {
  class Metal : public Material {
  public:
    sPtr<Texture> albedo;
    float         fuzz;

    Metal() = default;

    Metal(const vec3 &color, float f)
        : albedo(std::make_shared<SolidColor>(color)), fuzz(f < 1 ? f : 1) {}

    Metal(sPtr<Texture> tex) : albedo(tex) {}

    bool scatter(const Ray &r_in, HitRecord &rec, vec3 &attenuation,
                 Ray &scattered) const override {

      vec3 in_normalized = r_in.direction.Normalize();
      vec3 reflected     = in_normalized.Reflect(rec.normal);
      scattered =
          Ray(rec.p, reflected + vec3::RandomInUnitSphere() * fuzz, r_in.time);
      attenuation = albedo->Value(rec.u, rec.v, rec.p);
      return (vec3::DotProd(scattered.direction, rec.normal) > 0);
    }

    json GetJson() const override {
      return json{{"type", "metal"}, {"texture", albedo->GetJson()}};
    }
  };

  inline void from_json(const json &j, Metal &m) {
    m.fuzz   = j["fuzz"];
    m.albedo = TextureFactory::FromJson(j["texture"]);
  }

  inline void to_json(json &j, const Metal &m) { j = m.GetJson(); }
} // namespace rt