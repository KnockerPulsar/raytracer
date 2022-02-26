#include "Ray.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Material.h"
#include "Pixel.h"
#include "Camera.h" 
#include <chrono>

using std::chrono::high_resolution_clock,std::chrono::duration_cast;
using raytracer::Pixel, raytracer::Camera, raytracer::HittableList;

namespace raytracer {
  Vec3 Ray::RayColor(const raytracer::Ray &r, const Hittable &world,
                     int depth) {
    HitRecord rec;

    // Limit max recursion depth
    if (depth <= 0) {
      return Vec3::Zero();
    }

    if (world.Hit(r, 0.001f, infinity, rec)) {
      raytracer::Ray scattered;
      Vec3           attenuation;

      if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
        Vec3 rayClr = RayColor(scattered, world, depth - 1);
        return attenuation * rayClr;
      }

      return Vec3::Zero();
    }

    Vec3 unit_direction = r.direction.Normalize();

    float t = 0.5 * (unit_direction.y + 1.0);
    Vec3  startColor(1.0, 1.0, 1.0), endColor(0.5, 0.7, 1.0);

    return startColor * (1 - t) + endColor * t;
  }

  void Ray::Trace(std::vector<Pixel> &threadJobs, int jobsStart,
                       int jobsEnd, raytracer::Camera &cam, HittableList &world,
                       int max_depth, int image_width, int image_height,
                       int samples_per_pixel, long &thread_time,
                       int &thread_progress) {

    auto start = high_resolution_clock::now();
    for (int i = jobsStart; i < jobsEnd; i++) {

      Pixel &job = threadJobs[i];

      int x = job.x;
      int y = job.y;

      for (int s = 0; s < samples_per_pixel; s++) {
        float          u   = (x + RandomFloat()) / (image_width - 1);
        float          v   = (y + RandomFloat()) / (image_height - 1);
        raytracer::Ray ray = cam.GetRay(u, v);
        job.color += raytracer::Ray::RayColor(ray, world, max_depth);
      }

#ifdef GAMMA_CORRECTION
      // Gamma correction
      float r = job.color.x, g = job.color.y, b = job.color.z;

      float scale = 1.0 / samples_per_pixel;
      r           = sqrt(scale * r);
      g           = sqrt(scale * g);
      b           = sqrt(scale * b);

      job.color = Vec3(r, g, b);
#else
      job.color /= samples_per_pixel;
#endif
      thread_progress = ((float)(i - jobsStart) / (jobsEnd - jobsStart)) * 100;
    }
    auto stop = high_resolution_clock::now();
    thread_time =
        duration_cast<std::chrono::milliseconds>(stop - start).count();
  }

} // namespace raytracer