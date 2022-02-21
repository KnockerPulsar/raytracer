#include <array>
#include <iostream>

#include "Camera.h"
#include "Clr.h"
#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"
#include "raytracer.h"

#define title "Raytracer"

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

using raytracer::HittableList, raytracer::HitRecord, raytracer::Sphere,
    raytracer::Hittable;

typedef Color clr;

std::string ProgStr(int currVal, int maxVal) {
  return "[ " + std::to_string(currVal) + " / " + std::to_string(maxVal) + " ]";
}

std::string ProgBar(int currVal, int minVal, int maxVal, int maxChars) {
  float percentage = (float)currVal / (maxVal - minVal);
  int chars = (int)(percentage * maxChars);
  return std::string(chars, 'X');
}

Vec3 RayColor(const raytracer::Ray& r, const Hittable& world, int depth) {
  HitRecord rec;

  // Limit max recursion depth
  if (depth <= 0)
    return Vec3::Zero();

  if (world.Hit(r, 0.001f, infinity, rec)) {
    raytracer::Ray scattered;
    Vec3 attenuation;

    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      Vec3 rayClr = RayColor(scattered, world, depth - 1);
      return attenuation * rayClr;
    }

    return Vec3::Zero();
  }

  Vec3 unit_direction = r.direction.Normalize();

  float t = 0.5 * (unit_direction.y + 1.0);
  Vec3 startColor(1.0, 1.0, 1.0), endColor(0.5, 0.7, 1.0);

  return startColor * (1 - t) + endColor * t;
}

int main(void) {
  // Window
  const int image_width = 600;
  const float aspect_ratio = 16.0 / 9.0;
  const int image_height = (image_width / aspect_ratio);
  const int samples_per_pixel = 100;
  const int max_depth = 50;

  InitWindow(image_width, image_height, title);
  SetTargetFPS(60);

  // World
  HittableList world;

  auto material_ground =
      make_shared<raytracer::Lambertian>(Vec3(0.8, 0.8, 0.0));
  auto material_center =
      make_shared<raytracer::Lambertian>(Vec3(0.1, 0.2, 0.5));
  auto material_left = make_shared<raytracer::Dielectric>(1.5);
  auto material_right = make_shared<raytracer::Metal>(Vec3(0.8, 0.6, 0.2), 1.0);

  world.Add(make_shared<Sphere>(100.0, Vec3(0, -100.5, -1), material_ground))
      .Add(make_shared<Sphere>(0.5, Vec3(0, 0, -1.0), material_center))
      .Add(make_shared<Sphere>(0.5, Vec3(-1., 0, -1), material_left))
      .Add(make_shared<Sphere>(0.5, Vec3(1., 0, -1), material_right));

  // Camera
  raytracer::Camera cam;

  while (!WindowShouldClose()) {
    ClearBackground(MAGENTA);
    BeginDrawing();

    // Trace rays
    for (int y = image_height - 1; y >= 0; y--) {
      int currProgress = image_height - y + 1;
      std::cout << "\33[2K\r" << ProgBar(currProgress, 0, image_height - 1, 40)
                << ProgStr(currProgress, image_height - 1) << std::flush;

      for (int x = 0; x < image_width; x++) {
        Vec3 pixel_color(0, 0, 0);

        for (int s = 0; s < samples_per_pixel; s++) {
          float u = (x + RandomFloat()) / (image_width - 1);
          float v = (y + RandomFloat()) / (image_height - 1);
          raytracer::Ray r = cam.GetRay(u, v);

          pixel_color += RayColor(r, world, max_depth);
        }

#ifdef GAMMA_CORRECTION
        // Gamma correction
        auto [r, g, b] = pixel_color;

        float scale = 1.0 / samples_per_pixel;
        r = sqrt(scale * r);
        g = sqrt(scale * g);
        b = sqrt(scale * b);

        pixel_color = Vec3(r, g, b);
#else
        pixel_color /= samples_per_pixel;
#endif

        Clr clr = Clr::FromFloat(pixel_color.x, pixel_color.y, pixel_color.z);

        // This is since raylib starts the vertical axis at the top left
        // While the tutorial assumes it on the bottom right
        int raylib_y = image_height - y - 1;
        DrawPixel(x, raylib_y, clr);
      }
    }

    EndDrawing();
  }

  return 0;
}