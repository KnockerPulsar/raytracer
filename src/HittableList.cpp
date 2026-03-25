#include "HittableList.h"

#include "AABB.h"
#include "Defs.h"

#include <vector>

namespace rt {
  bool HittableList::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    HitRecord temp_rec;
    bool      hit_anything   = false;
    float     closest_so_far = t_max;

    for (const auto &obj : objects) {
      if (obj->HitTransformed(r, t_min, closest_so_far, temp_rec)) {
        hit_anything   = true;
        closest_so_far = temp_rec.t;
        rec            = temp_rec;
      }
    }
    return hit_anything;
  }

  void HittableList::add(sPtr<Hittable> new_child) { objects.push_back(new_child); }

  void HittableList::remove(sPtr<Hittable> child_to_remove) {
    auto [eraseStart, _] = std::ranges::remove(objects, child_to_remove);
    objects.erase(eraseStart);
  }

  std::vector<sPtr<Hittable>> HittableList::get_hittables() const { return objects; }

  void HittableList::regenerate() {
    // Don't need to do anything
  }
} // namespace rt
