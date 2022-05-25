#include "HittableList.h"
#include "AABB.h"
#include "Defs.h"
#include <vector>

namespace rt {
  bool HittableList::Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const

  {
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

  bool HittableList::BoundingBox(float t0, float t1, AABB &outputBox) const {
    if (objects.empty())
      return false;

    AABB tempBox;
    bool firstBox = true;

    for (const auto &obj : objects) {
      if (!obj->BoundingBox(t0, t1, tempBox))
        return false;

      outputBox = firstBox ? tempBox : AABB::SurroundingBox(outputBox, tempBox);
      firstBox  = false;
    }

    return true;
  }

  std::vector<Hittable*> HittableList::getChildrenAsList() {

    std::vector<Hittable*> vec;
    for (auto&& e : objects) {
      vec.push_back(e.get());
    }
    return vec;
  }

  std::vector<AABB> HittableList::getChildrenAABBs() {
    std::vector<AABB> vec;
    for (auto&& e : getChildrenAsList()) {
      AABB output;
      if(e->BoundingBoxTransformed(0, 1, output))
        vec.push_back(output);
    }
    return vec;
  }

} // namespace rt