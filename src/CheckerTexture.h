#pragma once
#include "SolidColor.h"
#include "Vec3.h"
#include <future>
#include <memory>
#include <raylib.h>

namespace raytracer {
  class CheckerTexture : public Texture {
  private:
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
    float                    scale = 1.0;

  public:
    CheckerTexture() = default;
    CheckerTexture(std::shared_ptr<Texture> _even,
                   std::shared_ptr<Texture> _odd, float s = 1.0f)
        : even(_even), odd(_odd), scale(s) {}

    CheckerTexture(Vec3 c1, Vec3 c2, float s = 1.0f)
        : even(std::make_shared<SolidColor>(c1)),
          odd(std::make_shared<SolidColor>(c2)), scale(s) {}

    virtual Vec3 Value(float u, float v, const Vec3 &p) const override {
      float sines = sin(scale * p.x) * sin(scale * p.y) * sin(scale * p.z);
      if (sines < 0)
        return odd->Value(u, v, p);
      else
        return even->Value(u, v, p);
    }
  };
} // namespace raytracer