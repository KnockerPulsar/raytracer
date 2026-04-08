#pragma once
#include "common/Defs.h"
#include "common/Hittable.h"
#include "Material.h"
#include "textures/SolidColor.h"

namespace rt::raytracer {

  /**
   * @brief Contains logic to model lambertian/diffuse materials
   */
  class Lambertian : public Material {
  public:
    Lambertian(sPtr<Texture> color) : albedo(color) {}
    Lambertian(const vec3 &color) : Lambertian(ms<SolidColor>(color)) {}

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
    virtual bool scatter(const rt::Ray &rIn, rt::HitRecord &rec, vec3 &attenuation, Ray &scattered) const override {

      vec3 scatterDir = rec.normal + vec3::RandomUnitVec();

      if (scatterDir.NearZero())
        scatterDir = rec.normal;

      scattered   = Ray(rec.p, scatterDir.Normalize(), rIn.time);
      attenuation = albedo->Value(rec.u, rec.v, rec.p);
      return true;
    }

  private:
    sPtr<Texture const> const albedo; // Base matte/diffuse texture
  };
} // namespace rt
