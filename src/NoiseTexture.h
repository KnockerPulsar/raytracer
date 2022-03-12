#include "Perlin.h"
#include "Texture.h"
#include <raylib.h>

namespace raytracer {
  class NoiseTexture : public raytracer::Texture {
  public:
    Perlin noise;
    float  scale;
    NoiseTexture() = default;
    NoiseTexture(float scl) : scale(scl){};

    virtual Vec3 Value(float u, float v, const Vec3 &p) const override {
      // The noise fn can return negative values
      // Scale it to between 0 and 1
      return Vec3(1, 1, 1) * 0.5 *
             (1.0 + sin(scale * p.z + 10 * noise.Turb(p)));
    }
  };
} // namespace raytracer