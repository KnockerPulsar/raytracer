#pragma once
#include "Vec3.h"

namespace raytracer {
  struct Pixel {
    int  x, y;
    Vec3 color;
  };
} // namespace raytracer