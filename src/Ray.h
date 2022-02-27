#pragma once

#include "Vec3.h"

namespace raytracer {

  class Hittable;
  class Camera;
  class HittableList;
  class Pixel;
  class Scene;

  class Ray {
  public:
    Vec3 origin, direction;

    Ray() = default;
    Ray(Vec3 org, Vec3 dir) : origin(org), direction(dir) {}
    Vec3 At(float t) const { return Vector3Add(origin, direction * t); }

    static Vec3 RayColor(const raytracer::Ray &r, const Hittable &world,
                         int depth);

    static void Trace(std::vector<Pixel> &threadJobs, int jobsStart,
                      int jobsEnd, Scene &currScene, long &thread_time,
                      int &thread_progress);
  };

} // namespace raytracer