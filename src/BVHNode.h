#pragma once
#include "AABB.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"

#include <cassert>
#include <cstddef>
#include <optional>
#include <vector>

using std::vector, std::optional, std::make_optional;

namespace rt {
  class BVHNode : public Hittable {
  public:
    sPtr<Hittable> left = nullptr, right = nullptr;
    AABB           box;

    BVHNode() = default;

    BVHNode(const vector<sPtr<Hittable>> &list, float t0, float t1);

    BVHNode(const HittableList &list, float t0, float t1);

    BVHNode(const vector<sPtr<Hittable>> &srcObjects, size_t start, size_t end, float t0, float t1);

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    static bool BoxCompare(const sPtr<Hittable> a, const sPtr<Hittable> b, int axis);

    static bool boxXCompare(const sPtr<Hittable> a, const sPtr<Hittable> b);

    static bool boxYCompare(const sPtr<Hittable> a, const sPtr<Hittable> b);

    static bool boxZCompare(const sPtr<Hittable> a, const sPtr<Hittable> b);

    // To use a shared pointer, you must have some reference to the shared pointer object itself
    // and not the pointer it's wrapping.
    vector<sPtr<Hittable>> getChildrenAsList() override;

    vector<AABB> getChildrenAABBs() override;

    // Use with newChild = nullptr to regenerate the tree
    Hittable *addChild(sPtr<Hittable> newChild) override;

    Hittable *removeChild(sPtr<Hittable> childToRemove) override;

    bool isLeaf() const;
  };

} // namespace rt
