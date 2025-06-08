#pragma once
#include "../Perlin.h"
#include "../data_structures/vec3.h"
#include "Texture.h"

#include <imgui.h>
#include <raylib.h>

#include <cmath>
#include <memory>
#include <optional>

namespace rt {
  class NoiseTexture : public rt::Texture {
  public:
    NoiseTexture() : noise{std::make_shared<Perlin>()} {}
    NoiseTexture(float scl, float tScl = 10, vec3 baseCol = vec3(1))
        : noise(std::make_shared<Perlin>()), scale(scl), turbScale(tScl), baseColor(baseCol) {}

    NoiseTexture(const json &json)
        : scale(json["scale"].get<float>()), turbScale(json["turb_scale"].get<float>()),
          baseColor(json["base_color"].get<vec3>()) {}

    virtual vec3 Value(float u, float v, const vec3 &p) const override {
      // The noise fn can return negative values
      // Scale it to between 0 and 1
      return (vec3(1, 1, 1) * 0.5 * (1.0 + sin(scale * (p.z) + turbScale * noise->Turb(p))) * baseColor) * multiplier;
    }

    virtual json toJson() const override {
      return json{{"type", "noise"}, {"scale", scale}, {"turb_scale", turbScale}, {"base_color", baseColor}};
    }


    // TODO: Better perlin preview
    // Main issue: result depends on the given point and not the uv coords
    // uvs give 2 degrees of freedom while points have 3 degrees of freedom
    // Can't use RNG since, well, it's random
    ::Texture generatePreview(int availableWidth, int availableHeight, float scale = 1.0f) override {
      return Texture::SamplePreview(
          [](float u, float v) { return v * u; }, std::nullopt, std::nullopt, availableWidth, availableHeight, scale
      );
    }

    virtual void OnImgui() override {

      ImGui::DragFloat("Noise scale", &scale, 0.1f);
      ImGui::DragFloat("Turbulance scale", &turbScale, 0.1f);
      ImGui::ColorEdit3("Base color", &baseColor.x);

      ImGui::Spacing();

      Texture::OnImgui();
    }

  private:
    sPtr<Perlin> const noise;
    float        scale;
    float        turbScale;
    vec3         baseColor;
  };

  inline void to_json(json &j, const NoiseTexture &nt) { j = json{{"type", "noise"}, nt.toJson()}; }
} // namespace rt
