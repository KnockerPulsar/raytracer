#include "HittableList.h"

#include "AABB.h"

namespace rt::raytracer {
  bool HittableList::hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
    rt::HitRecord temp_rec;
    bool      hit_anything   = false;
    float     closest_so_far = t_max;

    for (const auto &obj : objects) {
      if (obj->Hit(r, t_min, closest_so_far, temp_rec)) {
        hit_anything   = true;
        closest_so_far = temp_rec.t;
        rec            = temp_rec;
      }
    }
    return hit_anything;
  }
} // namespace rt
