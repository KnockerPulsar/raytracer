#pragma once
#include "common/Defs.h"
#include "../materials/Material.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "common/Hittable.h"
#include <memory>

namespace rt::raytracer {
  class Metal : public Material {
  public:
    Metal(sPtr<Texture> tex, float f = 0.5) : albedo(tex), fuzz(f) {}
    Metal(const vec3 &color, float f) : Metal(std::make_shared<SolidColor>(color), f) {}

    bool scatter(const rt::Ray &r_in, rt::HitRecord &rec, vec3 &attenuation, Ray &scattered) const override {
      vec3 inNormlized = r_in.direction.Normalize();
      vec3 reflected     = inNormlized.Reflect(rec.normal);
      scattered          = Ray(rec.p, reflected + vec3::RandomInUnitSphere() * fuzz, r_in.time);
      attenuation        = albedo->Value(rec.u, rec.v, rec.p);
      return (vec3::DotProd(scattered.direction, rec.normal) > 0);
    }
  private:
    sPtr<Texture const> const albedo;
    float const               fuzz;
  };
} // namespace rt
