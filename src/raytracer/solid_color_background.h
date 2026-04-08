#pragma once

#include "background.h"

namespace rt::raytracer {
  class SolidColorBackground : public Background {
  public:
    SolidColorBackground(vec3 backgroundColor) : backgroundColor(backgroundColor) {}

    auto sampleBackground(vec3 direction) -> vec3 override {
      return backgroundColor;
    }

  private:
    vec3 const backgroundColor;
  };
}
