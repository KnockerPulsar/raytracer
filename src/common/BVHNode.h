#pragma once
#include "AABB.h"
#include "Hittable.h"
#include "Ray.h"

#include <cassert>
#include <cstddef>

namespace rt {
  class Raytracable;
  namespace raytracer {

    struct BVHNodeData {
      BVHNodeData(std::vector<std::unique_ptr<Raytracable>> &srcObjects, size_t start, size_t end, float t0, float t1);
      std::unique_ptr<Hittable> left, right;
    };

    class BVHNode : public BVHNodeData, public rt::Hittable {
    public:
      BVHNode(std::vector<std::unique_ptr<Raytracable>> &list, float t0, float t1);
      BVHNode(std::vector<std::unique_ptr<Raytracable>> &srcObjects, size_t start, size_t end, float t0, float t1);

      ~BVHNode();

      virtual bool Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override;
    };

  } // namespace raytracer
} // namespace rt
