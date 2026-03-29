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
    sPtr<Texture> albedo;
    float         fuzz;

    Metal() = default;

    Metal(const vec3 &color, float f) : albedo(std::make_shared<SolidColor>(color)), fuzz(f < 1 ? f : 1) {}

    Metal(sPtr<Texture> tex) : albedo(tex) {}

    bool scatter(const rt::Ray &r_in, rt::HitRecord &rec, vec3 &attenuation, Ray &scattered) const override {

      vec3 inNormlized = r_in.direction.Normalize();
      vec3 reflected     = inNormlized.Reflect(rec.normal);
      scattered          = Ray(rec.p, reflected + vec3::RandomInUnitSphere() * fuzz, r_in.time);
      attenuation        = albedo->Value(rec.u, rec.v, rec.p);
      return (vec3::DotProd(scattered.direction, rec.normal) > 0);
    }
  };
} // namespace rt
