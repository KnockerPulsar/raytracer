#pragma once
#include "raytracer.h"

namespace raytracer
{
    class Camera
    {
    public:
        Camera()
        {
            float aspect_ratio = 16.0 / 9.0;
            float viewport_height = 2.0;
            float viewport_width = aspect_ratio * viewport_height;
            float focal_length = 1.0f;

            origin = vec3::Zero();
            horizontal = vec3(viewport_width);
            vertical = vec3(0, viewport_height);
            lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);
        }

        Ray GetRay(float u, float v)
        {
            return Ray(origin, lower_left_corner + horizontal * u + vertical * v - origin);
        }

    private:
        vec3 origin, lower_left_corner, horizontal, vertical;
    };
} // namespace raytracer
