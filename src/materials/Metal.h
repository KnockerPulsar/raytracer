#pragma once
#include "../Defs.h"
#include "../materials/Material.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include "../Hittable.h"
#include <memory>

namespace rt {
  class Metal : public Material {
  public:
    float         fuzz;

    Metal() = default;

    bool scatter(const Ray &r_in, HitRecord &rec, vec3 &attenuation, Ray &scattered) const override {

      vec3 inNormlized = r_in.direction.Normalize();
      vec3 reflected     = inNormlized.Reflect(rec.normal);
      scattered          = Ray(rec.p, reflected + vec3::RandomInUnitSphere() * fuzz, r_in.time);
      attenuation        = albedo->Value(rec.u, rec.v, rec.p);
      return (vec3::DotProd(scattered.direction, rec.normal) > 0);
    }

    json toJson() const override {
      return json{
          {"type", "metal"},
          {"fuzz", fuzz},
          {"texture", albedo->toJson()},
      };
    }

    virtual void OnImgui() override {
      albedo->OnImgui();
      ImGui::DragFloat("Fuzziness", &fuzz, 0.05, 0, 1);
    }
  };

  inline void from_json(const json &j, Metal &m) {
    m.fuzz   = j["fuzz"];
    m.albedo = TextureFactory::FromJson(j["texture"]);
  }

  inline void to_json(json &j, const Metal &m) { j = m.toJson(); }
} // namespace rt