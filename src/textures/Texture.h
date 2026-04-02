#pragma once

#include "../data_structures/vec3.h"

#include <raylib.h>
#include <imgui.h>


namespace rt::raytracer {
  class Texture {
  protected:
    float multiplier = 1.0f;

  public:
    Texture &operator=(Texture const &other) {
      multiplier     = other.multiplier;

      return *this;
    }

    virtual vec3 Value(float u, float v, const vec3 &p) const = 0;
    // virtual json toJson() const                               = 0;
    virtual void setIntensity(float i) { multiplier = i; }

    virtual ~Texture() = default;
  };
} // namespace rt
