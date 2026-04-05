#include "BVHNode.h"

#include "Util.h"
#include "common/Raytracable.h"
#include "common/AABB.h"
#include "data_structures/vec3.h"

#include <algorithm>
#include <memory>

namespace {
  bool BoxCompare(sPtr<const rt::Raytracable>& a, sPtr<const rt::Raytracable> &b, int axis) {
    auto const boxA = a->worldspaceBoundingBox;
    auto const boxB = b->worldspaceBoundingBox;

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

  bool boxXCompare(sPtr<const rt::Raytracable> a, sPtr<const rt::Raytracable> b) { return BoxCompare(a, b, 0); }
  bool boxYCompare(sPtr<const rt::Raytracable> a, sPtr<const rt::Raytracable> b) { return BoxCompare(a, b, 1); }
  bool boxZCompare(sPtr<const rt::Raytracable> a, sPtr<const rt::Raytracable> b) { return BoxCompare(a, b, 2); }

  rt::AABB GetAABB(std::shared_ptr<rt::Hittable> left, std::shared_ptr<rt::Hittable> right) {
    if (left == right) {
      return right->worldspaceBoundingBox;
    } else {
      return rt::AABB::SurroundingBox(left->worldspaceBoundingBox, right->worldspaceBoundingBox);
    }
  }

  auto RandomAxisComparator() {
    int axis = RandomInt(0, 2);
    switch (axis) {
    case 0:
      return boxXCompare;
    case 1:
      return boxYCompare;
    case 2:
      return boxZCompare;
    default:
      return boxXCompare;
    }
  }

  // Somehow this is about twice as slow in the Cornell box scene
  auto LongestAxisComparator(std::vector<sPtr<rt::Raytracable>> &srcObjects, size_t start, size_t end) {
    auto const infinity = rt::constants::infinity;
    auto [xMin, xMax] = std::pair{infinity, -infinity};
    auto [yMin, yMax] = std::pair{infinity, -infinity};
    auto [zMin, zMax] = std::pair{infinity, -infinity};
    auto const objSpan = std::span{srcObjects}.subspan(start, end - start);
    for(auto const& obj: objSpan) {
      auto const box = obj->worldspaceBoundingBox;
      xMin = fmin(xMin, box.min.x);
      yMin = fmin(yMin, box.min.y);
      zMin = fmin(zMin, box.min.z);

      xMax = fmax(xMax, box.max.x);
      yMax = fmax(yMax, box.max.y);
      zMax = fmax(zMax, box.max.z);
    }

    auto const xSpan = xMax - xMin;
    auto const ySpan = yMax - yMin;
    auto const zSpan = zMax - zMin;
    auto const biggestSpan = std::max({xSpan, ySpan, zSpan});

    if (biggestSpan == xSpan) {
      return boxXCompare;
    } else if (biggestSpan == ySpan) {
      return boxYCompare;
    } else {
      return boxZCompare;
    }
  }
} // namespace

// TODO this constructor can be rewritten to use spans
// TODO SAH
rt::raytracer::BVHNode::BVHNodeData::BVHNodeData(std::vector<sPtr<rt::Raytracable>> &srcObjects, size_t start,
                                                 size_t end, float t0, float t1) {
  using BVHNode = rt::raytracer::BVHNode;

  auto const comparator = RandomAxisComparator();

  auto const numberOfObjects = end - start;
  if (numberOfObjects == 1) {
    left = right = srcObjects[start];
  } else if (numberOfObjects == 2) {
    if (comparator(srcObjects[start], srcObjects[start + 1])) {
      left  = srcObjects[start];
      right = srcObjects[start + 1];
    } else {
      left  = srcObjects[start + 1];
      right = srcObjects[start];
    }
  } else {
    std::sort(srcObjects.begin() + start, srcObjects.begin() + end, comparator);

    auto mid = start + numberOfObjects / 2;
    left     = std::make_shared<BVHNode>(srcObjects, start, mid, t0, t1);
    right    = std::make_shared<BVHNode>(srcObjects, mid, end, t0, t1);
  }
}

rt::raytracer::BVHNode::BVHNode(std::vector<sPtr<Raytracable>> &list, float t0, float t1) : BVHNode(list, 0, list.size(), t0, t1) {}

rt::raytracer::BVHNode::BVHNode(std::vector<sPtr<Raytracable>> &srcObjects, size_t start, size_t end, float t0,
                                float t1)
    : BVHNodeData(srcObjects, start, end, t0, t1), Hittable(GetAABB(left, right)) {}

bool rt::raytracer::BVHNode::Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
  // Bounding box and ray are both in worldspace.
  if (!worldspaceBoundingBox.Hit(r, t_min, t_max))
    return false;

  bool leftHit  = left->Hit(r, t_min, t_max, rec);
  bool rightHit = right->Hit(r, t_min, leftHit ? rec.t : t_max, rec);

  return leftHit || rightHit;
}
