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
      union { vec3 O3 = vec3(0) ;  __m128 O4; } origin;
      union { vec3 D3 = vec3(0) ;  __m128 D4; } direction;
      union { vec3 rD3 = vec3(0); __m128 rD4; } rDirection;
      float time = 10e30;

    Ray() { origin.O4 = direction.D4 = rDirection.rD4 = _mm_set1_ps(1); };
    Ray(const vec3& o, const vec3& d) {
      origin.O3 = o;
      direction.D3 = d;
      rDirection.rD3 = { 1.0f / d.x, 1.0f / d.y, 1.0f / d.z };
    }

    Ray(const vec3& o, const vec3& d, float time) : Ray(o,d) { this->time = time; }

    vec3 At(float t) const { return origin.O3 + direction.D3 * t; }

    static vec3 RayColor(const rt::Ray &r, const Scene* scene, int depth);

    static void Trace(
      AsyncRenderData &ard,
      const Scene* scene,
      int threadIndex
    );

  };

} // namespace rt

// clang-format on