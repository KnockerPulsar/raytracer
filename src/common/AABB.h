#pragma once

#include "data_structures/vec3.h"
#include <raylib.h>
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

    void Pad();

    bool Hit(const Ray &r, float tMin, float tMax) const;
  };
} // namespace rt
