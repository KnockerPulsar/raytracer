#include <raylib.h>
#include "../include/raylib-cpp/include/raylib-cpp.hpp"

#include "sphere.h"

typedef raylib::Vector3 vec3;

namespace raytracer
{
    class Ray
    {

    public:
        vec3 origin, direction;

        Ray(vec3 org, vec3 dir) : origin(org), direction(dir) {}
        vec3 PointAtT(float t) { return origin + direction * t; }

        // https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/#:~:text=When%20the%20ray%20and%20sphere,and%20solving%20for%20%24t%24.
        float SphereIntersection(Sphere &sph)
        {
            // Vector between ray origin and sphere center
            vec3 oc = Vector3Subtract(origin, sph.center);
            float a = direction.DotProduct(direction);
            float b = oc.DotProduct(direction) * 2;
            float c = oc.DotProduct(oc) - sph.radius * sph.radius;
            float discriminant = b * b - 4 * a * c;

            if (discriminant < 0)
                return -1;
            else
            {
                float sqrtDiscriminant = sqrt(discriminant);
                float numerator = -b - sqrtDiscriminant;

                if (numerator > 0)
                    return numerator / (2 * a);

                numerator = -b + sqrtDiscriminant;
                if (numerator > 0)
                    return numerator / (2 * a);
                else
                    return -1;
            }
        }

        raylib::Color RayColor()
        {
            Sphere s = Sphere(0.5, vec3(0, 0, -1));
            float t = SphereIntersection(s);
            if (t > 0)
            {
                vec3 normal = (PointAtT(t) - vec3(0, 0, -1)).Normalize();

                return raylib::Color((normal.x + 1) * 255 / 2, (normal.y + 1) * 255 / 2, (normal.z + 1) * 255 / 2, 255);
            }

            vec3 unit_direction = direction.Normalize();
            t = (unit_direction.y + 1.0) * 0.5;
            vec3 startColor(1, 1, 1), endColor(0.5, 0.7, 1.0);
            vec3 lerpedColor = (startColor * (1 - t) + endColor * t) * 255;

            raylib::Color retColor = raylib::Color(lerpedColor.x, lerpedColor.y, lerpedColor.z, 255);
            return retColor;
        }
    };

}