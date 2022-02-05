#include <array>
#include <iostream>

#include "Sphere.h"

#include "raytracer.h"
#include "HittableList.h"

#define title "Raytracer"

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

using raylib::Window;
using raytracer::HittableList, raytracer::HitRecord, raytracer::Sphere, raytracer::Hittable;

typedef raylib::Color clr;

clr RayColor(const raytracer::Ray &r, const Hittable &world)
{
    HitRecord rec;
    if (world.Hit(r, 0.0f, infinity, rec))
    {
        vec3 normal_scaled = (rec.normal + vec3(1.0, 1.0, 1.0))*255*0.5;
        return clr(normal_scaled.x, normal_scaled.y, normal_scaled.z, 255);
    }

    vec3 unit_direction = r.direction.Normalize();

    float t = 0.5 * (unit_direction.y + 1.0);
    vec3 startColor(1.0, 1.0, 1.0), endColor(0.5, 0.7, 1.0);

    vec3 lerpedColor = startColor * (1 - t) + endColor * t;
    lerpedColor = lerpedColor * 255;

    return clr(lerpedColor.x,lerpedColor.y,lerpedColor.z, 255);
}

int main(void)
{
    // Window
    const int image_width = 400;
    const float aspect_ratio = 16.0 / 9.0;
    const int image_height = (image_width / aspect_ratio);

    Window window = Window(image_width, image_height, title);
    window.SetTargetFPS(60);

    // World
    HittableList world;
    world
        .Add(make_shared<Sphere>(0.5, vec3(0, 0, -1)))
        .Add(make_shared<Sphere>(100, vec3(0, -100.5, -1)));

    // Camera

    float viewport_height = 2.0;
    float viewport_width = viewport_height * aspect_ratio;
    float focal_length = 1.0;

    vec3 origin = vec3::Zero();
    vec3 horizontal = vec3(viewport_width, 0, 0);
    vec3 vertical = vec3(0, viewport_height, 0);
    vec3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    raytracer::Sphere sph1(2, vec3(0, 0, 50));

    while (!WindowShouldClose())
    {
        ClearBackground(MAGENTA);

        // Trace rays
        for (int y = image_height - 1; y >= 0; y--)
        {
            std::cout << "\rLines remaining: " << y << std::flush;
            for (int x = 0; x < image_width; x++)
            {
                float u = float(x) / (image_width - 1);
                float v = float(y) / (image_height - 1);

                raytracer::Ray r(origin, lower_left_corner + horizontal * u + vertical * v - origin);
                raylib::Color col = RayColor(r, world);

                // This is since raylib starts the vertical axis at the top left
                // While the tutorial assumes it on the bottom right
                int raylib_y = image_height - y - 1;
                DrawPixel(x, raylib_y, col);
            }
        }

        EndDrawing();
    }

    return 0;
}