#pragma once

#include "background.h"
#include <filesystem>

namespace rt::raytracer {
  class SkysphereBackground : public Background {
  public:
    SkysphereBackground(::Image const& skysphereImage);

    auto sampleBackground(vec3 direction) -> vec3 override;

  private:
    ::Image const skysphereImage;
  };
} // namespace rt::raytracer
