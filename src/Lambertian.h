#pragma once
#include "Defs.h"
#include "Hittable.h"
#include "Material.h"

using nlohmann::json;

namespace raytracer {

  class Lambertian : public Material {
  private:
    sPtr<Texture> albedo;

  public:
    Lambertian(const Vec3 &color)
        : albedo(std::make_shared<SolidColor>(color)) {}

    Lambertian(sPtr<Texture> color) : albedo(color) {}

    Lambertian(const sPtr<Lambertian> &l) : albedo(l->albedo) {}

    Lambertian(json materialJson) {
      if (materialJson["texture"]["type"].get<std::string>() == "solid_color") {
        Vec3 color = Vec3::FromJson(materialJson["texture"]["color"]);
        albedo     = std::make_shared<SolidColor>(color);
      }
    }

    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const override {

      Vec3 scatter_dir = rec.normal + Vec3::RandomUnitVec();

      if (scatter_dir.NearZero())
        scatter_dir = rec.normal;

      scattered   = Ray(rec.p, scatter_dir, r_in.time);
      attenuation = albedo->Value(rec.u, rec.v, rec.p);
      return true;
    }
  };
} // namespace raytracer