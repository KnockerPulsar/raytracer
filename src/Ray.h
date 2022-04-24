#pragma once

#include "data_structures/vec3.h"

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

    static vec3 RayColor(const rt::Ray &r, const vec3 &backgroundColor,
                         const Hittable &world, int depth);

    static void Trace(std::vector<Pixel> &threadJobs, int jobsStart,
                      int jobsEnd, Scene &currScene, long &thread_time,
                      int &thread_progress, int &threadShouldRun);
  };

} // namespace rt