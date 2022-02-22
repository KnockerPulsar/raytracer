#include <array>
#include <chrono>
#include <iostream>
#include <thread>

#include "Camera.h"
#include "Clr.h"
#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"
#include "raytracer.h"

#define title "Raytracer"
#define NUM_THREADS 12

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

using raytracer::HittableList, raytracer::HitRecord, raytracer::Sphere,
    raytracer::Hittable, raytracer::Dielectric, raytracer::Lambertian,
    raytracer::Metal;

using std::string, std::thread, std::vector, std::chrono::high_resolution_clock,
    std::chrono::duration_cast, std::chrono::milliseconds;

struct Pixel {
  int  x, y;
  Vec3 color;
};

string ProgStr(int currVal, int maxVal) {
  return "[ " + std::to_string(currVal) + " / " + std::to_string(maxVal) + " ]";
}

string ProgBar(int currVal, int minVal, int maxVal, int maxChars) {
  float percentage = (float)currVal / (maxVal - minVal);
  int   chars      = (int)(percentage * maxChars);
  return string(chars, 'X');
}

Vec3 RayColor(const raytracer::Ray &r, const Hittable &world, int depth) {
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

HittableList CreateWorld1() {
  HittableList world;

  auto material_ground = make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0));
  auto material_center = make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5));
  auto material_left   = make_shared<Dielectric>(1.5);
  auto material_right  = make_shared<Metal>(Vec3(0.8, 0.6, 0.2), 1.0);

  world //
      .Add(make_shared<Sphere>(100.0, Vec3(0, -100.5, -1), material_ground))
      .Add(make_shared<Sphere>(0.5, Vec3(0, 0, -1.0), material_center))

      // These 2 spheres make a "hollow sphere"
      .Add(make_shared<Sphere>(0.5, Vec3(-1., 0, -1), material_left))
      .Add(make_shared<Sphere>(-0.45, Vec3(-1., 0, -1), material_left))

      .Add(make_shared<Sphere>(0.5, Vec3(1., 0, -1), material_right));

  return world;
}

HittableList CreateWorld2() {

  float        R = cos(pi / 4);
  HittableList world;

  auto material_left  = make_shared<Lambertian>(Vec3(0, 0, 1));
  auto material_right = make_shared<Lambertian>(Vec3(1, 0, 0));

  world //
      .Add(make_shared<Sphere>(R, Vec3(-R, 0, -1), material_left))
      .Add(make_shared<Sphere>(R, Vec3(R, 0, -1), material_right));

  return world;
}

void Trace(std::vector<Pixel> &threadJobs, int jobsStart, int jobsEnd,
           raytracer::Camera &cam, HittableList &world, int max_depth,
           int image_width, int image_height, int samples_per_pixel,
           long &thread_time) {
  auto start = high_resolution_clock::now();
  for (int i = jobsStart; i < jobsEnd; i++) {

    Pixel &job = threadJobs[i];

    int x = job.x;
    int y = job.y;

    for (int s = 0; s < samples_per_pixel; s++) {
      float          u   = (x + RandomFloat()) / (image_width - 1);
      float          v   = (y + RandomFloat()) / (image_height - 1);
      raytracer::Ray ray = cam.GetRay(u, v);
      job.color += RayColor(ray, world, max_depth);
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
  }
  auto stop   = high_resolution_clock::now();
  thread_time = duration_cast<std::chrono::milliseconds>(stop - start).count();
}

int main(void) {
  // Window
  const int   image_width       = 320;
  const float aspect_ratio      = 16.0 / 9.0;
  const int   image_height      = (image_width / aspect_ratio);
  const int   samples_per_pixel = 20;
  const int   max_depth         = 10;

  InitWindow(image_width, image_height, title);
  SetTargetFPS(60); // Not like we're gonna hit it...

  // World
  HittableList world = CreateWorld1();

  // Camera
  Vec3 lookFrom = Vec3(-2, 2, 1);
  Vec3 moveDir  = Vec3(0.1, 0, 0);

  vector<thread> threads;
  vector<long>   thread_time(NUM_THREADS, 0);

  vector<Pixel> pixelJobs =
      vector(image_width * image_height, Pixel{0, 0, Vec3::Zero()});
  raytracer::Camera cam = raytracer::Camera(lookFrom, Vec3(0, 0, -1),
                                            Vec3(0, 1, 0), 45, aspect_ratio);

  // Prepare pixel jobs
  for (int y = 0; y < image_height; y++) {
    for (int x = 0; x < image_width; x++) {
      pixelJobs[x + image_width * y] = Pixel{x, y, Vec3::Zero()};
    }
  }

  while (!WindowShouldClose()) {

    if (lookFrom.x < -5)
      moveDir = Vec3(0.1, 0, 0);
    else if (lookFrom.x > 5)
      moveDir = Vec3(-0.1, 0, 0);

    lookFrom += moveDir * GetFrameTime();

    for (int t = 0; t < NUM_THREADS; t++) {
      int totalJobs = pixelJobs.size();
      int jobsStart = t * totalJobs / NUM_THREADS;
      int jobsEnd   = (t + 1) * totalJobs / NUM_THREADS;

      threads.push_back(thread(Trace, std::ref(pixelJobs), jobsStart, jobsEnd,
                               std::ref(cam), std::ref(world), max_depth,
                               image_width, image_height, samples_per_pixel,
                               std::ref(thread_time[t])));
    }

    for (int i = 0; i < NUM_THREADS; i++) {
      threads[i].join();
    }

    for (int i = 0; i < NUM_THREADS; i++) {
      std::cout << "Thread " << i << "\texecution time:\t" << thread_time[i]
                << " ms" << std::endl;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
      std::cout << "\033[A";
    }

    threads.clear();

    ClearBackground(MAGENTA);
    BeginDrawing();

    for (auto &&pixel : pixelJobs) {
      Clr clr     = Clr::FromFloat(pixel.color.x, pixel.color.y, pixel.color.z);
      pixel.color = Vec3::Zero();

      // This is since raylib starts the vertical axis at the top left
      // While the tutorial assumes it on the bottom right
      int raylib_y = image_height - pixel.y - 1;
      DrawPixel(pixel.x, raylib_y, clr);
    }

    EndDrawing();

    cam = raytracer::Camera(lookFrom, Vec3(0, 0, -1), Vec3(0, 1, 0), 45,
                            aspect_ratio);
  }

  return 0;
}
