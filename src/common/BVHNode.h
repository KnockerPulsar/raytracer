#pragma once
#include "AABB.h"
#include "Defs.h"
#include "Hittable.h"
#include "Ray.h"

#include <cassert>
#include <cstddef>

namespace rt::raytracer {

  struct BVHNodeData {
    BVHNodeData(const std::vector<sPtr<Hittable>> &srcObjects, size_t start, size_t end, float t0, float t1);

    sPtr<Hittable> left = nullptr, right = nullptr;
  };

  // TODO BVHNodes aren't really "hittables" as they don't really have a
  // transformation and material. To fix that, the state in `rt::Hittable`
  // could be moved to a `rt::Raytracable` class that implements it, and
  // hittable would then become just the interface.
  //
  // Spheres, Boxes, etc.. would implement `Raytracable`, BVHNode would
  // implement `Hittable`
  class BVHNode : public BVHNodeData, public rt::Hittable {

  public:
    BVHNode(const std::vector<sPtr<Hittable>> &srcObjects, size_t start, size_t end, float t0, float t1);

    virtual bool Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override;
  };

} // namespace rt
