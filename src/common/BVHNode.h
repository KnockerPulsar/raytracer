#pragma once
#include "AABB.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"

#include <cassert>
#include <cstddef>

namespace rt::raytracer {
  class BVHNode : public rt::Hittable {
  private:
    sPtr<Hittable> left = nullptr, right = nullptr;
    AABB           box;

  public:
    BVHNode(const std::vector<sPtr<Hittable>> &list, float t0, float t1);

    BVHNode(const std::vector<sPtr<Hittable>> &srcObjects, size_t start, size_t end, float t0, float t1);

    virtual bool Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    // std::vector<sPtr<Hittable>> getChildrenAsList() override;
    //
    // // TODO should this be removed?
    // std::vector<AABB> getChildrenAABBs() override;
    //
    // // Use with newChild = nullptr to regenerate the tree
    // Hittable *addChild(sPtr<Hittable> newChild) override;
    //
    // Hittable *removeChild(sPtr<Hittable> childToRemove) override;
  };

} // namespace rt
