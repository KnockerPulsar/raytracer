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
    vec3 min, max;

    AABB() = default;
    AABB(const vec3 &a, const vec3 &b);

    // Build an AABB from points
    AABB(std::vector<vec3> points);

    bool Hit(const Ray &r, float tMin, float tMax) const;
    static AABB SurroundingBox(AABB b0, AABB b1);
  };
} // namespace rt