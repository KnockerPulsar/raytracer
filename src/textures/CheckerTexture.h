#pragma once
#include "data_structures/vec3.h"
#include "SolidColor.h"
#include "Texture.h"

#include <imgui.h>
#include <raylib.h>

#include <memory>

namespace rt::raytracer {
  class CheckerTexture : public Texture {
  private:
    std::shared_ptr<Texture> const even;
    std::shared_ptr<Texture> const odd;
    float                    scale;

  public:
    CheckerTexture(std::shared_ptr<Texture> even_, std::shared_ptr<Texture> _odd, float s = 6.0f)
        : even(even_), odd(_odd), scale(s) {}

    virtual vec3 sample(float u, float v, const vec3 &p) const override {
      float sines = sin(scale * p.x) * sin(scale * p.y) * sin(scale * p.z);
      if (sines < 0)
        return odd->sample(u, v, p);

      return even->sample(u, v, p);
    }
  };
} // namespace rt
