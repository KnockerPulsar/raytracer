#pragma once

#include "../data_structures/vec3.h"

#include <raylib.h>
#include <imgui.h>

namespace rt::raytracer {
  class Texture {
  public:
    virtual ~Texture() = default;
    virtual vec3 sample(float u, float v, const vec3 &p) const = 0;
  };
} // namespace rt::raytracer
