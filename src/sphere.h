#include <raylib.h>
#include "../include/raylib-cpp/include/raylib-cpp.hpp"

namespace raytracer
{
#ifndef Sphere
    class Sphere
    {
    public:
        float radius;
        raylib::Vector3 center;
        Sphere(float r, raylib::Vector3 pos) : radius(r), center(pos) {}
    };
#endif

}