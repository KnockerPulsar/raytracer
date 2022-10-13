#pragma once

#include "Constants.h"
#include "data_structures/vec3.h"
#include <cmath>
#include <raylib.h>
#include <tuple>
#include <vector>

namespace rt {
  class Ray;

  class AABB {
  public:
    union {
      struct { vec3 min, max; } b3;
      struct { __m128 min, max; } b4;
    };

    AABB() { b3.min = b3.max = 0; }
    AABB(const vec3 &a, const vec3 &b);

    // Build an AABB from points
    AABB(std::vector<vec3> points);

    void Pad();

    bool Hit(const Ray &r, float tMin, float tMax) const;
    static AABB SurroundingBox(AABB b0, AABB b1);
  };
} // namespace rt