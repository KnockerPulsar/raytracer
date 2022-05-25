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
    float editorEmissiveIntensity = 1.0f;
    vec3  editorEmissiveColor;

  public:
    sPtr<Texture> emssiveTex; // Texture that emission follows (can be used as an emissive mask)
    enum EmissiveTexType { Solid, Tex };
    EmissiveTexType emssiveTexType;

    DiffuseLight() = default;

    DiffuseLight(sPtr<Texture> tex) : emssiveTex(tex) { emssiveTexType = Tex; }

    DiffuseLight(vec3 baseColor) : emssiveTex(ms<SolidColor>(baseColor)) {
      emssiveTexType      = Solid;
      editorEmissiveColor = baseColor;
    }

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

    virtual void OnImgui() override {
      switch (emssiveTexType) {
      case Solid: {
        SolidColor *colorTexture = dynamic_cast<SolidColor *>(emssiveTex.get());

        if (colorTexture != nullptr) {
          ImGui::ColorPicker3("Emissive Color", &editorEmissiveColor.x);
          ImGui::DragFloat("Emissive intensity", &editorEmissiveIntensity, 0.05f, 0.01, 100.0f);

          colorTexture->color = editorEmissiveColor * editorEmissiveIntensity;
        } else
          std::cerr << EditorUtils::string_format("%s:%s: color texture is null", __FILE__, __LINE__);
      }
      case Tex: {
        // TODO: figure this out?
      }
      }
    }
  };

  inline void from_json(const json &j, DiffuseLight &dl) { dl.emssiveTex = TextureFactory::FromJson(j["texture"]); }

  inline void to_json(json &j, const DiffuseLight &dl) { j = dl.GetJson(); }
} // namespace rt