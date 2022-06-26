#pragma once

#include "Defs.h"
#include "Hittable.h"

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

using ::std::make_shared;
using ::std::shared_ptr;

namespace rt {
  class HittableList : public Hittable {
  public:
    std::vector<shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(shared_ptr<Hittable> object) { Add(object); }

    void Clear() { objects.clear(); }

    // Kept for compatibility with existing code.
    HittableList &Add(shared_ptr<Hittable> object) {
      objects.push_back(object);
      return *this;
    }

    virtual Hittable *addChild(sPtr<Hittable> newChild) override { return &Add(newChild); }

    virtual Hittable *removeChild(sPtr<Hittable> childToRemove) override {

      // TODO: figure out a cleaner way
      std::vector<sPtr<Hittable>>::iterator toRemove = objects.end();
      for (auto it = objects.begin(); it != objects.end(); it++) {
        if (*it == childToRemove)
          toRemove = it;
      }

      if (toRemove != objects.end())
        objects.erase(toRemove);

      return this;
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    virtual std::vector<sPtr<Hittable>> getChildrenAsList() override;

    std::vector<AABB> getChildrenAABBs() override;
  };
} // namespace rt
