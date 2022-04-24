#pragma once
#include "../Defs.h"
#include "../Hittable.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include "Material.h"

namespace rt {
  /**
   * @brief Contains logic to model diffuse light material.
   */
  class DiffuseLight : public Material {
  public:
    sPtr<Texture> emssiveTex; // Texture that emission follows (can be used as an emissive mask)

    DiffuseLight() = default;

    DiffuseLight(sPtr<Texture> tex) : emssiveTex(tex) {}

    DiffuseLight(vec3 baseColor) : emssiveTex(ms<SolidColor>(baseColor)) {}

    /**
     * @brief Contains scattering logic. Light materials absorb all rays that hit them. (yes it's backwards)
     *
     * @param rIn Input ray
     * @param rec Input hit record
     * @param attenuation Accumulated texture effects during ray tracing
     * @param scattered Scattered/output ray
     * @return false Always as in real life lights only emit and not absorb, things are flipped with camera raytracing.
     */
    virtual bool scatter(const Ray &rIn, HitRecord &rec, vec3 &attenuation, Ray &scattered) const override {
      return false;
    }

    /**
     * @brief Emission logic, lights always emit lights based on their texture.
     *
     * @param u Horizontal texture coordinate
     * @param v Horizontal texture coordinate
     * @param p Hit point in world space
     * @return vec3 Color of light that this material emits
     */
    virtual vec3 emitted(float u, float v, const vec3 &p) const override { return emssiveTex->Value(u, v, p); }

    json GetJson() const override { return json{{"type", "diffuse_light"}, {"texture", emssiveTex->GetJson()}}; }
  };

  inline void from_json(const json &j, DiffuseLight &dl) { dl.emssiveTex = TextureFactory::FromJson(j["texture"]); }

  inline void to_json(json &j, const DiffuseLight &dl) { j = dl.GetJson(); }
} // namespace rt