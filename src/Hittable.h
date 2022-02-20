#pragma once

#include "Ray.h"
#include "raytracer.h"

namespace raytracer
{
    class Material;

    struct HitRecord
    {
        Vec3 p;
        Vec3 normal;
        shared_ptr<Material> mat_ptr;
        float t;
        bool front_face;

        inline void set_face_normal(const Ray &r, const Vec3 &outward_normal)
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
