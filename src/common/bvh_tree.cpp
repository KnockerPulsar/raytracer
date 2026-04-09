#include "bvh_tree.h"

namespace rt::raytracer {
  BVHTree::BVHTree(std::vector<std::unique_ptr<Raytracable>> &&list, float t0, float t1)
      : root{std::make_unique<BVHNode>(list, 0, list.size(), t0, t1)}, t0{t0}, t1{t1} {}

  bool BVHTree::hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
    return root->Hit(r, t_min, t_max, rec);
  }
} // namespace rt
