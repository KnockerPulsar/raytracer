#pragma once

#include "../include/raylib-cpp/include/raylib-cpp.hpp"
#include "Ray.h"

namespace raytracer
{

    struct HitRecord
    {
        raylib::Vector3 p;
        raylib::Vector3 normal;
        float t;
        bool front_face;

        inline void set_face_normal(const Ray &r, const raylib::Vector3 &outward_normal)
        {
            front_face = Vector3DotProduct(outward_normal, r.direction) < 0;
            normal = front_face ? outward_normal : outward_normal * -1;
        }
    };

    class Hittable
    {
    public:
        virtual bool Hit(
            const Ray &r,
            float t_min,
            float t_max,
            HitRecord &rect) const = 0;
    };
} // namespace raytracer
