#include "BVHNode.h"

#include "Util.h"
#include "common/Raytracable.h"
#include "data_structures/vec3.h"

#include <algorithm>
#include <memory>

namespace {
  rt::AABB ToWorldspace(const rt::AABB &aabb, const rt::Transformation &transform) {
    // Generate all 8 vertices of the input AABB
    // Apply the transform to all 8
    // Get the bounding box of the rotated bounding box
    std::vector<vec3> vertices = {
        aabb.min,                                 // 000
        vec3(aabb.min.x, aabb.min.y, aabb.max.z), // 001
        vec3(aabb.min.x, aabb.max.y, aabb.min.z), // 010
        vec3(aabb.min.x, aabb.max.y, aabb.max.z), // 011
        vec3(aabb.max.x, aabb.min.y, aabb.min.z), // 100
        vec3(aabb.max.x, aabb.min.y, aabb.max.z), // 101
        vec3(aabb.max.x, aabb.max.y, aabb.min.z), // 110
        aabb.max,                                 // 111
    };

    for (auto &&vert : vertices) {
      vert = transform.Apply(vert);
    }

    vec3 newMin = vec3(rt::constants::infinity);
    vec3 newMax = -newMin;

    for (auto &&vert : vertices) {
      newMin.x = fmin(newMin.x, vert.x);
      newMin.y = fmin(newMin.y, vert.y);
      newMin.z = fmin(newMin.z, vert.z);

      newMax.x = fmax(newMax.x, vert.x);
      newMax.y = fmax(newMax.y, vert.y);
      newMax.z = fmax(newMax.z, vert.z);
    }

    return rt::AABB(newMin, newMax);
  }

  bool BoxCompare(sPtr<const rt::Raytracable>& a, sPtr<const rt::Raytracable> &b, int axis) {
    auto const boxA = ToWorldspace(a->boundingBox, a->transformation);
    auto const boxB = ToWorldspace(b->boundingBox, b->transformation);

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

  rt::AABB SurroundingBox(rt::AABB b0, rt::AABB b1) {
    vec3 smol(fmin(b0.min.x, b1.min.x), fmin(b0.min.y, b1.min.y), fmin(b0.min.z, b1.min.z));
    vec3 big(fmax(b0.max.x, b1.max.x), fmax(b0.max.y, b1.max.y), fmax(b0.max.z, b1.max.z));

    return rt::AABB(smol, big);
  }

  rt::AABB GetAABB(std::shared_ptr<rt::Hittable> left, std::shared_ptr<rt::Hittable> right) {
    auto const raytracableWorldspaceBoundingBox =
        [](auto const &node) {
          if (auto const leafRaytracable = std::dynamic_pointer_cast<rt::Raytracable>(node))
            return std::optional{ToWorldspace(leafRaytracable->boundingBox, leafRaytracable->transformation)};
          return std::optional<rt::AABB>{};
        };

    // Leaf node.. Bounding box is in local space
    if (left == right) {
      return *raytracableWorldspaceBoundingBox(left);
    } else {
      auto worldspaceBoundingBox = [&](auto const &node) {
        // Raytracable node bounding boxes are in local space
        return raytracableWorldspaceBoundingBox(node)
            .transform([](auto const &box) { return box; })
            // BVH node bounding boxes are in worldspace.
            .value_or(node->boundingBox);
      };
      auto const boxLeft  = worldspaceBoundingBox(left);
      auto const boxRight = worldspaceBoundingBox(right);

      return SurroundingBox(boxLeft, boxRight);
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
      auto const box = ToWorldspace(obj->boundingBox, obj->transformation);
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

  auto const comparator = LongestAxisComparator(srcObjects, start, end);

  auto const numberOfObjects = end - start;
  if (numberOfObjects == 1) {
    left = right = srcObjects[start];
  } else if (numberOfObjects == 1) {
    if (comparator(srcObjects[start], srcObjects[start + 1])) {
      left  = srcObjects[start];
      right = srcObjects[start + 1];
    } else {
      left  = srcObjects[start + 1];
      right = srcObjects[start];
    }
  } else {
    // TODO we're doing a bunch of matrix math in the comparator. Can we
    // compute the worldspace AABB for all objects at the start and just reuse
    // that?
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
  if (!boundingBox.Hit(r, t_min, t_max))
    return false;

  bool leftHit  = left->Hit(r, t_min, t_max, rec);
  bool rightHit = right->Hit(r, t_min, leftHit ? rec.t : t_max, rec);

  return leftHit || rightHit;
}
