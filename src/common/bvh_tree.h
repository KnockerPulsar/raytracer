#pragma once

#include "BVHNode.h"
#include <memory>

namespace rt::raytracer {
  class BVHTree : public AccelerationStructure {
  public:
    BVHTree(std::vector<sPtr<Hittable>> &&list, float t0, float t1);

    bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    void add(std::shared_ptr<Hittable> new_child) override;
    void remove(std::shared_ptr<Hittable> to_delete) override;
    auto get_hittables() const -> std::vector<std::shared_ptr<Hittable>> override;
    void regenerate() override;
  private:
    std::unique_ptr<BVHNode> root;
    float const t0, t1;
  };
} // namespace rt
