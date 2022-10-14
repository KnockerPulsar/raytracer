#include "Ray.h"
#include "AsyncRenderData.h"
#include "Camera.h"
#include "Constants.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Scene.h"
#include "app.h"
#include "data_structures/Pixel.h"
#include "materials/Material.h"
#include "data_structures/JobQueue.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>

using rt::Pixel, rt::Camera, rt::HittableList;
using std::chrono::high_resolution_clock, std::chrono::duration_cast;

namespace rt {
  vec3 Ray::RayColor(const rt::Ray &r, const Scene* scene, int depth) {
    HitRecord rec;

    // Limit max recursion depth
    if (depth <= 0) {
      return vec3::Zero();
    }

    if (!scene->worldRoot->Hit(r, 0.001f, infinity, rec)) {
      if (scene->skysphere)
        scene->skysphere->Hit(r, -infinity, infinity, rec);
      else
        return scene->backgroundColor;
    }

    rt::Ray scattered;
    vec3    attenuation;
    vec3    emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return emitted;

    return emitted + attenuation * RayColor(scattered, scene, depth - 1);
  }

  void Ray::Trace(AsyncRenderData &ard, const Scene* scene, int threadIndex) {

    while (true) {
      auto [jobsStart, jobsEnd] = ard.pixelJobs->getChunk(ard, threadIndex);
      float progressStep = 1.0f / (jobsEnd - jobsStart);
      ard.threadProgress[threadIndex] = 0;

      for (auto currentJob = jobsStart; currentJob != jobsEnd; ++currentJob) {

#ifdef FAST_EXIT
        // Exit prematurely if signaled to
        if (ard.exit == true)
          return;
#endif

        Pixel      &job       = *currentJob;

        int x = job.x;
        int y = job.y;

        float rWidth = 1.0f / (App::getImageWidth() - 1);
        float rHeight = 1.0f / (App::getImageHeight() - 1);

        for (int s = 0; s < scene->samplesPerPixel; s++) {
          float   u   = (x + RandomFloat()) * rWidth;
          float   v   = (y + RandomFloat()) * rHeight;
          rt::Ray ray = scene->cam.GetRay(u, v);
          job.color += rt::Ray::RayColor(ray, scene, scene->maxDepth);
        }

#ifdef GAMMA_CORRECTION
        // Gamma correction
        float scale = 1.0 / scene->samplesPerPixel;
        float r     = sqrt(scale * job.color.x);
        float g     = sqrt(scale * job.color.y);
        float b     = sqrt(scale * job.color.z);

        job.color = vec3(r, g, b);
#else
        job.color /= float(scene->settings.samplesPerPixel);
#endif
        ard.threadProgress[threadIndex] += progressStep;
      }
    }
  }
} // namespace rt