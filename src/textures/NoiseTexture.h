#pragma once
#include "imgui.h"
#include "../Perlin.h"
#include "../data_structures/vec3.h"

#include "Texture.h"
#include "Util.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <raylib.h>

namespace rt {
  class NoiseTexture : public rt::Texture {
  public:
    sPtr<Perlin> noise;
    float        scale;
    float        turbScale;
    vec3         baseColor;

    NoiseTexture() { noise = std::make_shared<Perlin>(); };
    NoiseTexture(float scl, float tScl = 10, vec3 baseCol = vec3(1)) : scale(scl), turbScale(tScl), baseColor(baseCol) {
      noise = std::make_shared<Perlin>();
    }

    virtual vec3 Value(float u, float v, const vec3 &p) const override {
      // The noise fn can return negative values
      // Scale it to between 0 and 1
      return (vec3(1, 1, 1) * 0.5 * (1.0 + sin(scale * (p.z) + turbScale * noise->Turb(p))) * baseColor) * multiplier;
    }

    virtual json toJson() const override {
      return json{{"type", "noise"}, {"scale", scale}, {"turb_scale", turbScale}, {"base_color", baseColor}};
    }

    virtual void GetTexture(const json &j) override {
      scale     = j["scale"].get<float>();
      turbScale = j["turb_scale"].get<float>();
      baseColor = j["base_color"].get<vec3>();
    }

    // TODO: Better perlin preview
    // Main issue: result depends on the given point and not the uv coords
    // uvs give 2 degrees of freedom while points have 3 degrees of freedom
    // Can't use RNG since, well, it's random
    void generatePreview() override {
      Texture::generatePreviewUtil([](float u, float v) { return v * u; });
    }

    virtual void OnImgui() override {

      ImGui::DragFloat("Noise scale", &scale, 0.1f);
      ImGui::DragFloat("Turbulance scale", &turbScale, 0.1f);
      ImGui::ColorEdit3("Base color", &baseColor.x);

      ImGui::Spacing();

      Texture::previewOrGenerate();
    }
  };

  inline void from_json(const json &j, NoiseTexture &nt) { nt.GetTexture(j); }
  inline void to_json(json &j, const NoiseTexture &nt) { j = json{{"type", "noise"}, nt.toJson()}; }
} // namespace rt
