#pragma once
#include "Perlin.h"
#include "Texture.h"
#include "Vec3.h"
#include <raylib.h>

namespace raytracer {
  class NoiseTexture : public raytracer::Texture {
  public:
    Perlin noise;
    float  scale;
    float  turbScale;
    Vec3   baseColor;

    NoiseTexture() = default;
    NoiseTexture(float scl, float tScl = 10, Vec3 baseCol = Vec3(1))
        : scale(scl), turbScale(tScl), baseColor(baseCol){};

    virtual Vec3 Value(float u, float v, const Vec3 &p) const override {
      // The noise fn can return negative values
      // Scale it to between 0 and 1
      return Vec3(1, 1, 1) * 0.5 *
             (1.0 + sin(scale * p.z + turbScale * noise.Turb(p))) * baseColor;
    }
  };
} // namespace raytracer