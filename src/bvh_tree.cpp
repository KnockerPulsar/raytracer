#include "bvh_tree.h"

namespace rt {
  BVHTree::BVHTree(std::vector<sPtr<Hittable>> &&list, float t0, float t1)
      : root{std::make_unique<BVHNode>(list, 0, list.size(), t0, t1)}, t0{t0}, t1{t1} {}

  bool BVHTree::hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    return root->Hit(r, t_min, t_max, rec);
  }

  void BVHTree::add(std::shared_ptr<Hittable> new_child) {
    auto objects = root->getChildrenAsList();
    objects.push_back(new_child);
    root.reset(new BVHNode(objects, t0, t1));
  }

  void BVHTree::remove(std::shared_ptr<Hittable> to_delete) {
    auto objects = root->getChildrenAsList();
    if (auto const iter = std::ranges::find(objects, to_delete); iter != objects.end()) {
      objects.erase(iter);
      root.reset(new BVHNode(objects, t0, t1));
    }
  }

  auto BVHTree::get_hittables() const -> std::vector<std::shared_ptr<Hittable>> { return root->getChildrenAsList(); }

  void BVHTree::regenerate() {
    auto objects = root->getChildrenAsList();
    root.reset(new BVHNode(objects, t0, t1));
  }
} // namespace rt
