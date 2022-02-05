#pragma once

#include "Hittable.h"

#include <memory>
#include <vector>

using ::std::make_shared;
using ::std::shared_ptr;

namespace raytracer
{
    class HittableList : public Hittable
    {
    public:
        std::vector<shared_ptr<Hittable>> objects;

        HittableList() {}
        HittableList(shared_ptr<Hittable> object) { Add(object); }

        void Clear() { objects.clear(); }

        HittableList &Add(shared_ptr<Hittable> object)
        {
            objects.push_back(object);
            return *this;
        }

        bool Hit(const Ray &r,
                 float t_min,
                 float t_max,
                 HitRecord &rec) const;
    };

    bool HittableList::Hit(
        const Ray &r,
        float t_min,
        float t_max,
        HitRecord &rec) const

    {
        HitRecord temp_rec;
        bool hit_anything = false;
        float closest_so_far = t_max;

        for (const auto &obj : objects)
        {
            if (obj->Hit(r, t_min, closest_so_far, temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }
        return hit_anything;
    }
} // namespace raytracer
