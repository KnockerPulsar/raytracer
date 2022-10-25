#pragma once
#include "../Defs.h"
#include "../Hittable.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include "Material.h"
#include <iostream>

namespace rt {
  /**
   * @brief Contains logic to model diffuse light material.
   */
  class DiffuseLight : public Material {
  private:
    float emissiveIntensity = 1.0;

  public:

    DiffuseLight() = default;

    /**
     * @brief Contains scattering logic. Light materials absorb all rays that hit them. (yes it's backwards)
     *
     * @param rIn Input ray
     * @param rec Input hit record
     * @param attenuation Accumulated texture effects during ray tracing
     * @param scattered Scattered/output ray
     * @return false Always as in real life lights only emit and not absorb, things are flipped with camera raytracing.
     */
    virtual bool scatter(const Ray &rIn, HitRecord &rec, vec3 &alb, Ray &scattered, float& pdf) const override{
      return false;
    }

    /**
     * @brief Emission logic, lights always emit lights based on their texture.
     *
     * @param u Horizontal texture coordinate
     * @param v Vertical texture coordinate
     * @param p Hit point in world space
     * @return vec3 Color of light that this material emits
     */
    virtual vec3 emitted(float u, float v, const vec3 &p) const override { return albedo->Value(u, v, p); }

    json toJson() const override { return json{{"type", "diffuse_light"}, {"texture", albedo->toJson()}}; }

    virtual void OnDerivedImgui() override {
      ImGui::DragFloat("Emissive intensity", &emissiveIntensity, 0.1, 1.0f);
      albedo->setIntensity(emissiveIntensity);
    }
  };

  inline void from_json(const json &j, DiffuseLight &dl) { dl.albedo = TextureFactory::FromJson(j["texture"]); }

  inline void to_json(json &j, const DiffuseLight &dl) { j = dl.toJson(); }
} // namespace rt
