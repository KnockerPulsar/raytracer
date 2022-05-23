#include "Ray.h"
#include "Camera.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Scene.h"
#include "data_structures/Pixel.h"
#include "materials/Material.h"
#include <atomic>
#include <chrono>
#include <iostream>

using rt::Pixel, rt::Camera, rt::HittableList;
using std::chrono::high_resolution_clock, std::chrono::duration_cast;

namespace rt {
  vec3 Ray::RayColor(const rt::Ray &r, const vec3 &backgroundColor,
                     const Hittable &world, int depth) {
    HitRecord rec;

    // Limit max recursion depth
    if (depth <= 0) {
      return vec3::Zero();
    }

    if (!world.Hit(r, 0.001f, infinity, rec))
      return backgroundColor;

    rt::Ray scattered;
    vec3           attenuation;
    vec3           emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return emitted;

    return emitted +
           attenuation * RayColor(scattered, backgroundColor, world, depth - 1);
  }

  void Ray::Trace(std::vector<Pixel> &threadJobs, int jobsStart, int jobsEnd,
                  Scene &currScene, long &thread_time, int &thread_progress,
                  int &threadShouldRun) {

    auto start = high_resolution_clock::now();
    for (int i = jobsStart; i < jobsEnd; i++) {

#ifdef FAST_EXIT
      // Exit prematurely if signaled to
      if (threadShouldRun == 0)
        return;
#endif

      Pixel &job = threadJobs[i];

      int x = job.x;
      int y = job.y;

      for (int s = 0; s < currScene.samplesPerPixel; s++) {
        float          u   = (x + RandomFloat()) / (currScene.imageWidth - 1);
        float          v   = (y + RandomFloat()) / (currScene.imageHeight - 1);
        rt::Ray ray = currScene.cam.GetRay(u, v);
        job.color += rt::Ray::RayColor(ray,
                                              currScene.backgroundColor,
                                              *currScene.worldRoot,
                                              currScene.maxDepth);
      }

#ifdef GAMMA_CORRECTION
      // Gamma correction
      float r = job.color.x, g = job.color.y, b = job.color.z;

      float scale = 1.0 / currScene.samplesPerPixel;
      r           = sqrt(scale * r);
      g           = sqrt(scale * g);
      b           = sqrt(scale * b);

      job.color = vec3(r, g, b);
#else
      job.color /= currScene.samplesPerPixel;
#endif
      thread_progress = ((float)(i - jobsStart) / (jobsEnd - jobsStart)) * 100;
    }

    auto stop = high_resolution_clock::now();
    thread_time =
        duration_cast<std::chrono::milliseconds>(stop - start).count();
  }

} // namespace rt