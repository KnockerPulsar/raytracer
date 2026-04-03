#include "HittableList.h"

#include "AABB.h"
#include "common/raycast.h"

namespace rt::raytracer {
  bool HittableList::hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
    rt::HitRecord tempRec;
    bool      hitAnything   = false;
    float     closestSoFar = t_max;

    for (const auto &obj : objects) {
      if (obj->Hit(r, t_min, closestSoFar, tempRec)) {
        hitAnything  = true;
        closestSoFar = tempRec.t;
        rec          = tempRec;
      }
    }
    return hitAnything;
  }
} // namespace rt
