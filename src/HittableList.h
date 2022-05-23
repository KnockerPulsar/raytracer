#pragma once

#include "Defs.h"
#include "Hittable.h"

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

    HittableList &Add(shared_ptr<Hittable> object) {
      objects.push_back(object);
      return *this;
    }

    virtual std::optional<sPtr<Hittable>> addChild(Hittable* newChild) override {
      objects.push_back(sPtr<Hittable>(newChild));
      return std::make_optional(sPtr<Hittable>(this));
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    virtual std::vector<Hittable*> getChildrenAsList()  override;

    std::vector<AABB> getChildrenAABBs() override;
  };
} // namespace rt
