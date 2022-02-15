#include <array>
#include <iostream>

#include "Sphere.h"

#include "Camera.h"
#include "HittableList.h"
#include "Material.h"
#include "raytracer.h"

#define title "Raytracer"

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

using raylib::Window;
using raytracer::HittableList, raytracer::HitRecord, raytracer::Sphere,
    raytracer::Hittable;

typedef raylib::Color clr;

vec3 RayColor(const raytracer::Ray& r, const Hittable& world, int depth) {
  HitRecord rec;

  // Limit max recursion depth
  if (depth <= 0)
    return vec3(0, 0, 0);

  if (world.Hit(r, 0.001f, infinity, rec)) {
    raytracer::Ray scattered;
    vec3 attenuation;

    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * RayColor(scattered, world, depth - 1);

    return vec3::Zero();
  }

  vec3 unit_direction = r.direction.Normalize();

  float t = 0.5 * (unit_direction.y + 1.0);
  vec3 startColor(1.0, 1.0, 1.0), endColor(0.5, 0.7, 1.0);

  return startColor * (1 - t) + endColor * t;
}

int main(void) {
  // Window
  const int image_width = 600;
  const float aspect_ratio = 16.0 / 9.0;
  const int image_height = (image_width / aspect_ratio);
  const int samples_per_pixel = 25;
  const int max_depth = 12;

  Window window = Window(image_width, image_height, title);
  window.SetTargetFPS(60);

  // World
  HittableList world;

  auto material_ground =
      make_shared<raytracer::Lambertian>(vec3(0.8, 0.8, 0.0));
  auto material_center =
      make_shared<raytracer::Lambertian>(vec3(0.7, 0.3, 0.3));
  auto material_left = make_shared<raytracer::Metal>(vec3(0.8, 0.8, 0.8), 0.3);
  auto material_right = make_shared<raytracer::Metal>(vec3(0.8, 0.6, 0.2), 1.0);

  world.Add(make_shared<Sphere>(100.0, vec3(0, -100.5, -1), material_ground))
      .Add(make_shared<Sphere>(0.5, vec3(0, 0, -1.0), material_center))
      .Add(make_shared<Sphere>(0.5, vec3(-1., 0, -1), material_left))
      .Add(make_shared<Sphere>(0.5, vec3(1., 0, -1), material_right));

  // Camera
  raytracer::Camera cam;

  while (!WindowShouldClose()) {
    ClearBackground(MAGENTA);
    BeginDrawing();

    // Trace rays
    for (int y = image_height - 1; y >= 0; y--) {
      std::cout << "\rLines remaining: " << y << std::flush;
      for (int x = 0; x < image_width; x++) {
        vec3 pixel_color(0, 0, 0);

        for (int s = 0; s < samples_per_pixel; s++) {
          float u = (x + random_float()) / (image_width - 1);
          float v = (y + random_float()) / (image_height - 1);
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

        pixel_color = vec3(r, g, b);
#else
        pixel_color /= samples_per_pixel;
#endif

        pixel_color *= 255;
        clr clr(pixel_color.x, pixel_color.y, pixel_color.z, 255);

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