#pragma once
#include "../Perlin.h"
#include "../data_structures/vec3.h"
#include "Texture.h"
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
    NoiseTexture(float scl, float tScl = 10, vec3 baseCol = vec3(1))
        : scale(scl), turbScale(tScl), baseColor(baseCol){};

    virtual vec3 Value(float u, float v, const vec3 &p) const override {
      // The noise fn can return negative values
      // Scale it to between 0 and 1
      return (vec3(1, 1, 1) * 0.5 * (1.0 + sin(scale * p.z + turbScale * noise->Turb(p))) * baseColor) * intensity;
    }

    virtual json GetJson() const override {
      return json{{"type", "noise"}, {"scale", scale}, {"turb_scale", turbScale}, {"base_color", baseColor}};
    }

    virtual void GetTexture(const json &j) override {
      scale     = j["scale"].get<float>();
      turbScale = j["turb_scale"].get<float>();
      baseColor = j["base_color"].get<vec3>();
    }

    virtual void OnImgui() override {
      // TODO: figure a way out to preview perlin noise
    }
  };

  inline void from_json(const json &j, NoiseTexture &nt) { nt.GetTexture(j); }
  inline void to_json(json &j, const NoiseTexture &nt) { j = json{{"type", "noise"}, nt.GetJson()}; }
} // namespace rt