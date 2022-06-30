#include "HittableList.h"
#include "AABB.h"
#include "Defs.h"
#include <optional>
#include <vector>

namespace rt {
  bool HittableList::Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
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

  Hittable *HittableList::addChild(sPtr<Hittable> newChild) { return &Add(newChild); }

  Hittable *HittableList::removeChild(sPtr<Hittable> childToRemove) {

    // TODO: figure out a cleaner way
    vector<sPtr<Hittable>>::iterator toRemove = objects.end();
    for (auto it = objects.begin(); it != objects.end(); it++) {
      if (*it == childToRemove)
        toRemove = it;
    }

    if (toRemove != objects.end())
      objects.erase(toRemove);

    return this;
  }

  vector<sPtr<Hittable>> HittableList::getChildrenAsList() {

    vector<sPtr<Hittable>> vec;
    for (auto &&e : objects) {
      vec.push_back(e);
    }
    return vec;
  }

  vector<AABB> HittableList::getChildrenAABBs() {
    vector<AABB> vec;

    for (auto &&e : getChildrenAsList()) {
      AABB output;
      if (e->BoundingBox(0, 1, output))
        vec.push_back(output);
    }
    return vec;
  }

} // namespace rt