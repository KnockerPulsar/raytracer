#pragma once
#include "AABB.h"
#include "Defs.h"
#include "Hittable.h"
#include "Ray.h"

#include <cassert>
#include <cstddef>

namespace rt {
  class Raytracable;
  namespace raytracer {

    struct BVHNodeData {
      BVHNodeData(std::vector<sPtr<Raytracable>> &srcObjects, size_t start, size_t end, float t0, float t1);
      sPtr<Hittable> left = nullptr, right = nullptr;
    };
    class BVHNode : public BVHNodeData, public rt::Hittable {
    public:
      BVHNode(std::vector<sPtr<Raytracable>> &list, float t0, float t1);
      BVHNode(std::vector<sPtr<Raytracable>> &srcObjects, size_t start, size_t end, float t0, float t1);

      virtual bool Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override;
    };

  } // namespace raytracer
} // namespace rt
