#include "Ray.h"

#include "AsyncRenderData.h"
#include "Camera.h"
#include "Constants.h"
#include "Hittable.h"
#include "Scene.h"
#include "Util.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"
#include "materials/Material.h"

#include <chrono>

using std::chrono::high_resolution_clock, std::chrono::duration_cast;

namespace rt {
  vec3 Ray::RayColor(const rt::Ray &r, const Scene* scene, int depth) {
    HitRecord rec;

    // Limit max recursion depth
    if (depth <= 0) {
      return vec3::Zero();
    }

    if (!scene->worldRoot->Hit(r, 0.001f, rt::constants::infinity, rec)) {
      if (scene->skysphere)
        scene->skysphere->Hit(r, -rt::constants::infinity, rt::constants::infinity, rec);
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
      auto start = high_resolution_clock::now();
      auto [jobsStart, jobsEnd] = ard.pixelJobs->getChunk(ard, threadIndex);

      for (auto currentJob = jobsStart; currentJob != jobsEnd; ++currentJob) {

#ifdef FAST_EXIT
        // Exit prematurely if signaled to
        if (ard.exit == true)
          return;
#endif

        Pixel      &job       = *currentJob;

        int x = job.x;
        int y = job.y;

          for (int s = 0; s < scene->settings.samplesPerPixel; s++) {
          float   u   = (x + RandomFloat()) / (scene->imageWidth - 1);
          float   v   = (y + RandomFloat()) / (scene->imageHeight - 1);
          rt::Ray ray = scene->cam.GetRay(u, v);
          job.color += rt::Ray::RayColor(ray, scene, scene->settings.maxDepth);
        }

#ifdef GAMMA_CORRECTION
        // Gamma correction
        float r = job.color.x, g = job.color.y, b = job.color.z;

        float scale = 1.0 / scene->settings.samplesPerPixel;
        r           = sqrt(scale * r);
        g           = sqrt(scale * g);
        b           = sqrt(scale * b);

        job.color = vec3(r, g, b);
#else
        job.color /= float(scene->settings.samplesPerPixel);
#endif
        ard.threadProgress[threadIndex] = ((float)(currentJob - jobsStart) / (jobsEnd - jobsStart)) * 100;
      }
      auto stop                    = high_resolution_clock::now();
      auto batchTime               = duration_cast<std::chrono::milliseconds>(stop - start).count();
      ard.threadTimes[threadIndex] += batchTime;
    }
  }
} // namespace rt
