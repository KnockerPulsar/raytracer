#pragma once

#include "Defs.h"
#include "Hittable.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

using ::std::make_shared;
using ::std::shared_ptr, std::optional, std::make_optional;

namespace rt {
  class HittableList : public Hittable {
  public:
    std::vector<sPtr<Hittable>> objects;

    HittableList() {}
    HittableList(sPtr<Hittable> object) { Add(object); }

    void Clear() { objects.clear(); }

    // Kept for compatibility with existing code.
    HittableList &Add(sPtr<Hittable> object) {
      objects.push_back(object);
      return *this;
    }

    virtual Hittable *addChild(sPtr<Hittable> newChild) override;

    virtual Hittable *removeChild(sPtr<Hittable> childToRemove) override;

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    virtual vector<sPtr<Hittable>> getChildrenAsList() override;

    vector<AABB> getChildrenAABBs() override;
  };
} // namespace rt
