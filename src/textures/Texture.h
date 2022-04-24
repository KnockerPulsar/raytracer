#pragma once
#include "../data_structures/vec3.h"
#include <raylib.h>

namespace rt {
  class Texture {
  public:
    virtual vec3 Value(float u, float v, const vec3 &p) const = 0;
    virtual json GetJson() const                              = 0;
    virtual void GetTexture(const json &j)                    = 0;
  };
} // namespace rt