#pragma once
#include "../Defs.h"
#include "../data_structures/vec3.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include "Hittable.h"
#include "Material.h"
#include <memory>

namespace rt {


  /**
   * @brief Contains logic to model dielectric (glass) materials.
   */
  class Dielectric : public Material {
  public:
    float         refractionIndex; // Controls how likely a ray is to reflect or refract

    Dielectric() = default;

   /**
     * @brief Hit logic, simulates dielectric/glass materials
     *
     * @param rIn Input ray
     * @param rec Input hit record
     * @param attenuation Accumulated texture effects during ray tracing
     * @param scattered Scattered/output ray
     * @return true Always, independent of whether the ray reflects or refracts
     */
    virtual bool scatter(const Ray &rIn, HitRecord &rec, vec3 &attenuation, Ray &scattered) const override {

      attenuation  = albedo->Value(rec.u, rec.v, rec.p);
      float refIdx = rec.front_face ? (1 / refractionIndex) : refractionIndex;

      vec3  unitDir  = rIn.direction.Normalize();
      float cosTheta = fmin(vec3::DotProd(-unitDir, rec.normal), 1.0);
      float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

      bool  cannotRefract = refIdx * sinTheta > 1.0;
      float reflectance   = Reflectance(cosTheta, refIdx);
      vec3  dir;

      // Either reflect when the refraction index and sinTheta are large
      // enought Or just reflect randomly. Otherwise, refract.
      if (cannotRefract || reflectance > RandomFloat())
        dir = unitDir.Reflect(rec.normal);
      else
        dir = unitDir.Refract(rec.normal, refIdx);

      scattered = Ray(rec.p, dir, rIn.time);
      return true;
    }

    virtual json toJson() const override {
      // clang-format off
      return json{
        {"type", "dielectric"},
        {"refraction_index", refractionIndex},
        {"texture", albedo->toJson()}
      };
      // clang-format on
    }

    virtual void OnImgui() override {
      albedo->OnImgui();
      ImGui::DragFloat("Refraction Index", &refractionIndex, 0.01, 1, 100);
    }

  private:
    /**
     * @brief Uses Schlick's approximation to calculate reflectance.
     *
     * @param cos Cos the angle between the incident and normal vectors (dot product)
     * @param refIdx Refraction index of the material
     * @return float The reflectence of the material (how likely it is to reflect a ray)
     */
    static float Reflectance(float cos, float refIdx) {
      float r0 = (1 - refIdx) / (1 + refIdx);
      r0       = r0 * r0;

      return r0 + (1 - r0) * pow(1 - cos, 5);
    }

  };

  inline void from_json(const json &j, Dielectric &d) {
    d.refractionIndex = j["refraction_index"].get<float>();
    d.albedo          = TextureFactory::FromJson(j["texture"]);
  }

  inline void to_json(json &j, const Dielectric &d) { j = d.toJson(); }
} // namespace rt