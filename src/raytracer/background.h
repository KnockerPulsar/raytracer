#pragma once

#include "data_structures/vec3.h"
namespace rt::raytracer {
  class Background {
  public:
    virtual ~Background() = default;

    virtual auto sampleBackground(vec3 direction) -> vec3 = 0;
  };
} // namespace rt::raytracer
