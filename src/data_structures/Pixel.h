#pragma once
#include "vec3.h"

namespace rt {
  /**
   * @brief Stores information used for parallelized computations
   */
  struct Pixel {
    int  x;     // X position of the pixel in screen-space
    int  y;     // Y position ...
    vec3 color; // Pixel color in RGB
  };
} // namespace rt