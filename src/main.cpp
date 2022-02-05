#include <array>
#include <iostream>

#include <raylib.h>
#include "../include/raylib-cpp/include/raylib-cpp.hpp"
#include "ray.h"
#include "canvas.h"

#define title "Raytracer"

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis
int main(void)
{
    const int image_width = 400;
    const float aspect_ratio = 16.0 / 9.0;
    const int image_height = (image_width / aspect_ratio);

    InitWindow(image_width, image_height, title);
    SetTargetFPS(60);

    float viewport_height = 2.0;
    float viewport_width = viewport_height * aspect_ratio;
    float focal_length = 1.0;

    raylib::Vector3 origin = raylib::Vector3::Zero();
    raylib::Vector3 horizontal = raylib::Vector3(viewport_width, 0, 0);
    raylib::Vector3 vertical = raylib::Vector3(0, viewport_height, 0);
    raylib::Vector3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - raylib::Vector3(0, 0, focal_length);

    raytracer::Sphere sph1(2, raylib::Vector3(0, 0, 50));

    while (!WindowShouldClose())
    {
        BeginDrawing();
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
                raylib::Color col = r.RayColor();

                // This is since raylib starts the vertical axis at the top left
                // While the tutorial assumes it on the bottom right
                int raylib_y = image_height - y;
                DrawPixel(x, raylib_y, col);
            }
        }

        EndDrawing();
    }

    return 0;
}