#pragma once

#include "AsyncRenderData.h"
// #include "data_structures/JobQueue.h"
// #include "data_structures/vec3.h"

// clang-format off

namespace rt {

  class Hittable;
  class Camera;
  class HittableList;
  class Pixel;
  class Scene;

  class Ray {
  public:
    vec3  origin, direction;
    float time;

    Ray() = default;
    Ray(vec3 org, vec3 dir) : origin(org), direction(dir) {}
    Ray(vec3 org, vec3 dir, float time = 0.0)
        : origin(org), direction(dir), time(time) {}

    vec3 At(float t) const { return Vector3Add(origin, direction * t); }

    static vec3 RayColor(const rt::Ray &r, Scene& scene, int depth);

    static void Trace(
      AsyncRenderData& ard,
      int threadIndex
    );
  };

} // namespace rt

// clang-format on