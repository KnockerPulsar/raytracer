#include <algorithm>
#include <exception>
#include <future>
#include <iostream>
#include <raylib.h>

#include "Camera.h"
#include "Clr.h"
#include "Pixel.h"
#include "Scenes.h"

#define title "Raytracer"
#define NUM_THREADS 12

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

using raytracer::HitRecord, raytracer::Sphere, raytracer::Hittable,
    raytracer::Dielectric, raytracer::Lambertian, raytracer::Metal,
    raytracer::Clr, raytracer::Pixel;

using std::string, std::thread, std::vector, std::chrono::high_resolution_clock,
    std::chrono::duration_cast, std::chrono::milliseconds, std::pair,
    std::future, std::tuple;

int main() {
  // Window
  const int   image_width       = 1280;
  const float aspect_ratio      = 16.0 / 9.0;
  const int   image_height      = (image_width / aspect_ratio);
  const int   samples_per_pixel = 100;
  const int   max_depth         = 50;
  const int   fov               = 60;

  InitWindow(image_width, image_height, title);
  SetTargetFPS(60); // Not like we're gonna hit it...

  // World
  raytracer::HittableList world = raytracer::CreateScene1();

  // Camera
  Vec3 lookFrom = Vec3(-2, 0.5, 1);
  Vec3 moveDir  = Vec3(0.1, 0, 0);

  vector<int>                      threadProgress = vector(NUM_THREADS, 0);
  vector<pair<bool, future<void>>> threads;
  vector<long>                     thread_time(NUM_THREADS, 0);

  vector<Pixel> pixelJobs =
      vector(image_width * image_height, Pixel{0, 0, Vec3::Zero()});
  raytracer::Camera cam = raytracer::Camera(lookFrom, Vec3(0, 0, -1),
                                            Vec3(0, 1, 0), fov, aspect_ratio);

  // Prepare pixel jobs
  for (int y = 0; y < image_height; y++) {
    for (int x = 0; x < image_width; x++) {
      pixelJobs[x + image_width * y] = Pixel{x, y, Vec3::Zero()};
    }
  }

  bool all_finished = true;
  bool fullscreen   = false;

  if (fullscreen)
    ToggleFullscreen();

  while (true) {

    BeginDrawing();

    if (IsKeyPressed(KEY_ESCAPE)) {
      break;
    }

    if (threads.size() == 0) {

      if (lookFrom.x < -5)
        moveDir = Vec3(0.1, 0, 0);
      else if (lookFrom.x > 5)
        moveDir = Vec3(-0.1, 0, 0);

      lookFrom += moveDir * GetFrameTime();

      for (int t = 0; t < NUM_THREADS; t++) {
        int totalJobs = pixelJobs.size();
        int jobsStart = t * totalJobs / NUM_THREADS;
        int jobsEnd   = (t + 1) * totalJobs / NUM_THREADS;

        threads.push_back(std::make_pair(
            false,
            std::async(std::launch::async, raytracer::Ray::Trace,
                       std::ref(pixelJobs), jobsStart, jobsEnd, std::ref(cam),
                       std::ref(world), max_depth, image_width, image_height,
                       samples_per_pixel, std::ref(thread_time[t]),
                       std::ref(threadProgress[t]))));
      }
    }

    all_finished = true;
    std::cout << "\r";
    for (int i = 0; i < NUM_THREADS; i++) {
      auto finished = threads[i].second.wait_for(milliseconds());

      if (finished == std::future_status::ready && threads[i].first == false)
        threads[i].first = true;

      all_finished &= threads[i].first;

      std::cout << std::to_string(threadProgress[i]) << "\t";
    }

    if (all_finished) {
      threads.clear();

      ClearBackground(MAGENTA);

      for (auto &&pixel : pixelJobs) {
        Clr clr = Clr::FromFloat(pixel.color.x, pixel.color.y, pixel.color.z);
        pixel.color = Vec3::Zero();

        // This is siRayColornce raylib starts the vertical axis at the top left
        // While the tutorial assumes it on the bottom right
        int raylib_y = image_height - pixel.y - 1;
        DrawPixel(pixel.x, raylib_y, clr);
      }

      cam = raytracer::Camera(lookFrom, Vec3(0, 0, -1), Vec3(0, 1, 0), fov,
                              aspect_ratio);
    }
    EndDrawing();
  }

  if (fullscreen)
    ToggleFullscreen();

  CloseWindow();

  for (auto& thread : threads) {
    thread.second.wait();
  }
  
  return 0;
}
