#pragma once

#include "data_structures/vec3.h"

namespace rt {
  class Ray {
  public:
    vec3  origin, direction;
    float time;

    Ray() = default;
    Ray(vec3 org, vec3 dir, float time = 0.0) : origin(org), direction(dir), time(time) {}

    vec3 At(float t) const { return Vector3Add(origin, direction * t); }
  };

} // namespace rt
