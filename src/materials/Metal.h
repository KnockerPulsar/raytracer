#pragma once
#include "../Defs.h"
#include "../materials/Material.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include <memory>

namespace raytracer {
  class Metal : public Material {
  public:
    sPtr<Texture> albedo;
    float         fuzz;

    Metal(const Vec3 &color, float f)
        : albedo(std::make_shared<SolidColor>(color)), fuzz(f < 1 ? f : 1) {}

    Metal(sPtr<Texture> tex) : albedo(tex) {}

    Metal(json materialJson) {
      fuzz   = materialJson["fuzz"].get<float>();
      albedo = TextureFactory::FromJson(materialJson["texture"]);
    }

    bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                 Ray &scattered) const override {

      Vec3 in_normalized = r_in.direction.Normalize();
      Vec3 reflected     = in_normalized.Reflect(rec.normal);
      scattered =
          Ray(rec.p, reflected + Vec3::RandomInUnitSphere() * fuzz, r_in.time);
      attenuation = albedo->Value(rec.u, rec.v, rec.p);
      return (Vec3::DotProd(scattered.direction, rec.normal) > 0);
    }
  };
} // namespace raytracer