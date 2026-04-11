#pragma once

#include "data_structures/vec3.h"

namespace rt {
  /**
   * @brief Stores information used for parallelized computations
   */
  struct Pixel {
    int const      x; // X position of the pixel in screen-space
    int const      y; // Y position ...
    uint64_t const seed;

    vec3 color; // Pixel color in RGB
  };
} // namespace rt
