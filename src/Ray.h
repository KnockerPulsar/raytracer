#pragma once

#include <raylib.h>
#include "raytracer.h"

namespace raytracer {
class Ray {
 public:
  Vec3 origin, direction;

  Ray(){}
  Ray(Vec3 org, Vec3 dir) : origin(org), direction(dir) {}
  Vec3 At(float t) const { return Vector3Add(origin, direction * t); }
};

}  // namespace raytracer