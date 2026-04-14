#pragma once
#include "common/Perlin.h"
#include "data_structures/vec3.h"
#include "Texture.h"

#include <imgui.h>
#include <raylib.h>

#include <cmath>
#include <memory>

namespace rt::raytracer {
  class NoiseTexture : public Texture {
  public:
    NoiseTexture(float scl, float tScl = 10, vec3 baseCol = vec3(1))
        : noise(std::make_shared<Perlin>()), scale(scl), turbScale(tScl), baseColor(baseCol) {}

    virtual vec3 sample(float u, float v, const vec3 &p) const override {
      // The noise fn can return negative values
      // Scale it to between 0 and 1
      return (vec3(1, 1, 1) * 0.5 * (1.0 + sin(scale * (p.z) + turbScale * noise->Turb(p))) * baseColor);
    }

  private:
    sPtr<Perlin> const noise;
    float        scale;
    float        turbScale;
    vec3         baseColor;
  };
} // namespace rt
