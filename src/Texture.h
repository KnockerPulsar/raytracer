#pragma once
#include <raylib.h>
#include "Vec3.h"
#include "Clr.h"

namespace raytracer {
  class Texture {
    public:
    virtual Vec3  Value(float u, float v, const Vec3& p) const = 0;
  };
} // namespace raytracer