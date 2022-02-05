#pragma once

#include <raylib.h>
#include "raytracer.h"

typedef raylib::Vector3 vec3;

namespace raytracer
{
    class Ray
    {

    public:
        vec3 origin, direction;

        
        Ray(vec3 org, vec3 dir) : origin(org), direction(dir) {}
        vec3 At(float t) const { return Vector3Add(origin, direction * t); }

        vec3 Origin() const { return origin; }

        
    };

}