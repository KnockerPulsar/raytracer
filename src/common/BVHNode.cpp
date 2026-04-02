#include "BVHNode.h"

#include "Util.h"
#include "data_structures/vec3.h"

#include <algorithm>
#include <iostream>
#include <memory>

namespace {
  bool BoxCompare(sPtr<const rt::Hittable> a, sPtr<const rt::Hittable> b, int axis) {
    auto const &boxA = a->boundingBox;
    auto const &boxB = b->boundingBox;

    switch (axis) {
    // X
    case 0:
      return boxA.min.x < boxB.min.x;
    // Y
    case 1:
      return boxA.min.y < boxB.min.y;

    // Z
    case 2:
      return boxA.min.z < boxB.min.z;

    _:
      return false;
    }
    return false;
  }

  bool boxXCompare(sPtr<const rt::Hittable> a, sPtr<const rt::Hittable> b) { return BoxCompare(a, b, 0); }
  bool boxYCompare(sPtr<const rt::Hittable> a, sPtr<const rt::Hittable> b) { return BoxCompare(a, b, 1); }
  bool boxZCompare(sPtr<const rt::Hittable> a, sPtr<const rt::Hittable> b) { return BoxCompare(a, b, 2); }
} // namespace

rt::raytracer::BVHNodeData::BVHNodeData(const std::vector<sPtr<Hittable>> &srcObjects, size_t start, size_t end,
                                        float t0, float t1) {

  if (srcObjects.empty()) {
    left  = nullptr;
    right = nullptr;
    return;
  }

  std::vector<sPtr<Hittable>> objs;
  // If root node, need to filter only at root
  if (start == 0 && end == srcObjects.size()) {
    for (auto &&e : srcObjects) { // If not a bvh node, add to list
      if (auto *bvh = dynamic_cast<BVHNode *>(e.get()); bvh == nullptr)
        objs.push_back(sPtr<Hittable>(e));
    }
    end = objs.size();
  } else {
    objs = srcObjects;
  }

  int  axis       = RandomInt(0, 2);
  auto comparator = (axis == 0) ? boxXCompare : (axis == 1) ? boxYCompare : boxZCompare;

  size_t objSpan = end - start;

  if (objSpan == 1) {
    left = right = objs[start];
  } else if (objSpan == 2) {
    if (comparator(objs[start], objs[start + 1])) {
      left  = objs[start];
      right = objs[start + 1];
    } else {
      left  = objs[start + 1];
      right = objs[start];
    }
  } else {
    std::sort(objs.begin() + start, objs.begin() + end, comparator);

    auto mid = start + objSpan / 2;
    left     = std::make_shared<BVHNode>(objs, start, mid, t0, t1);
    right    = std::make_shared<BVHNode>(objs, mid, end, t0, t1);
  }
}

rt::raytracer::BVHNode::BVHNode(const std::vector<sPtr<Hittable>> &srcObjects, size_t start, size_t end, float t0,
                                float t1)
    : BVHNodeData(srcObjects, start, end, t0, t1),
      Hittable(Transformation{}, nullptr, AABB::SurroundingBox(left->boundingBox, right->boundingBox)) {}

bool rt::raytracer::BVHNode::Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
  if (!boundingBox.Hit(r, t_min, t_max))
    return false;

  bool leftHit  = left->Hit(r, t_min, t_max, rec);
  bool rightHit = right->Hit(r, t_min, leftHit ? rec.t : t_max, rec);

  return leftHit || rightHit;
}
