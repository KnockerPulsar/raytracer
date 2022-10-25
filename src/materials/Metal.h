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
  
    Metal(float initialFuzz) : fuzz(initialFuzz) {}

    virtual bool scatter(const Ray &rIn, HitRecord &rec, vec3 &alb, Ray &scattered, float& pdf) const {
      vec3 inNormlized = rIn.direction.Normalize();
      vec3 reflected     = inNormlized.Reflect(rec.normal);
      scattered          = Ray(rec.p, reflected + vec3::RandomInUnitSphere() * fuzz, rIn.time);
      alb        = albedo->Value(rec.u, rec.v, rec.p);
      return (vec3::DotProd(scattered.direction, rec.normal) > 0);
    }

    json toJson() const override {
      return json{
          {"type", "metal"},
          {"fuzz", fuzz},
          {"texture", albedo->toJson()},
      };
    }

    virtual void OnDerivedImgui() override {
      ImGui::DragFloat("Fuzziness", &fuzz, 0.05, 0, 1);
    }
  
    Metal& setFuzz(float newFuzz) { 
      fuzz = newFuzz; 
      return *this;
    }
  
  };

  inline void from_json(const json &j, Metal &m) {
    m.fuzz   = j["fuzz"];
    m.albedo = TextureFactory::FromJson(j["texture"]);
  }

  inline void to_json(json &j, const Metal &m) { j = m.toJson(); }
} // namespace rt
