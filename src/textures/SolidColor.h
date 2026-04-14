#pragma once
#include "data_structures/vec3.h"
#include "Texture.h"

#include <nlohmann-json/json.hpp>
#include <raylib.h>


namespace rt::raytracer {
  class SolidColor : public Texture {
  public:
    SolidColor(vec3 c) : color(c) {}

    virtual vec3 Value(float u, float v, const vec3 &p) const override { return color * multiplier; }
  private:
    vec3 color;
  };
} // namespace rt
