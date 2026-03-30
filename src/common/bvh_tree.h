#pragma once

#include "BVHNode.h"

#include "common/acceleration_structure.h"

#include <memory>

namespace rt::raytracer {
  class BVHTree : public AccelerationStructure {
  public:
    BVHTree(std::vector<sPtr<Hittable>> &&list, float t0, float t1);

    bool hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override;
  private:
    std::unique_ptr<BVHNode> root;
    float const t0, t1;
  };
} // namespace rt
