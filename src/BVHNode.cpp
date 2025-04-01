#include "BVHNode.h"

#include "Util.h"
#include "data_structures/vec3.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_set>

namespace {
  bool BoxCompare(sPtr<const rt::Hittable> a, sPtr<const rt::Hittable> b, int axis) {
    rt::AABB boxA;
    rt::AABB boxB;

    if (!a->BoundingBox(0, 0, boxA) || !b->BoundingBox(0, 0, boxB))
      std::cerr << "No bounding box in BVHNode constructor.\n";
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

rt::BVHNode::BVHNode(const vector<sPtr<Hittable>> &list, float t0, float t1) : BVHNode(list, 0, list.size(), t0, t1) {}

rt::BVHNode::BVHNode(const HittableList &list, float t0, float t1)
    : BVHNode(list.objects, 0, list.objects.size(), t0, t1) {}

rt::BVHNode::BVHNode(const vector<sPtr<Hittable>> &srcObjects, size_t start, size_t end, float t0, float t1) {

  if (srcObjects.empty()) {
    left  = nullptr;
    right = nullptr;
    box   = {vec3::Zero(), vec3::Zero()};
    return;
  }

  vector<sPtr<Hittable>> objs;
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

  AABB boxLeft;
  AABB boxRight;

  if (!left->BoundingBox(t0, t1, boxLeft) || !right->BoundingBox(t0, t1, boxRight))
    std::cerr << "No bounding box in BVHNode constructor.\n";

  box = AABB::SurroundingBox(boxLeft, boxRight);
}

bool rt::BVHNode::Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
  if (!box.Hit(r, t_min, t_max))
    return false;

  bool leftHit  = left->HitTransformed(r, t_min, t_max, rec);
  bool rightHit = right->HitTransformed(r, t_min, leftHit ? rec.t : t_max, rec);

  // if(leftHit || rightHit)
  //   std::cout << "AAAA";
  return leftHit || rightHit;
}
bool rt::BVHNode::BoundingBox(float t0, float t1, AABB &outputBox) const {
  outputBox = box;
  return true;
}

std::vector<sPtr<rt::Hittable>> rt::BVHNode::getChildrenAsList() {
  std::unordered_set<sPtr<Hittable>> children;

  // Otherwise, its an itermediate BVH node, get its left and right children
  if (left != nullptr) {

    // May god forgive me for I'm about to sin
    auto leftBVH = std::dynamic_pointer_cast<BVHNode>(left);

    // If the cast failed and the the left child is not null
    // Then the child is not an intermediate BVHNode, but a leaf Hittable.
    // Add it to the list.

    if (leftBVH == nullptr) {
      children.insert(left);
    } else {
      auto leftChildren = left->getChildrenAsList();
      children.insert(leftChildren.begin(), leftChildren.end());
      // children.insert(left); // Add the intermediate node itself
    }
  }

  if (right != nullptr) {
    auto rightBVH = std::dynamic_pointer_cast<BVHNode>(right);

    // Same but for the right branch
    if (rightBVH == nullptr && right != nullptr)
      children.insert(right);
    else {
      auto rightChildren = right->getChildrenAsList();
      children.insert(rightChildren.begin(), rightChildren.end());
      // children.insert(right); // Add the intermediate node itself
    }
  }

  return std::vector<sPtr<rt::Hittable>>(children.begin(), children.end());
}

vector<rt::AABB> rt::BVHNode::getChildrenAABBs() {
  vector<sPtr<Hittable>> children = getChildrenAsList();
  // This is destroyed on function return causing a double

  vector<AABB> childrenAABBs;

  for (auto &&e : children) {
    AABB output;
    if (e->BoundingBox(0, 1, output))
      childrenAABBs.push_back(output);
  }

  return childrenAABBs;
}

rt::Hittable *rt::BVHNode::addChild(sPtr<Hittable> newChild) {
  vector<sPtr<Hittable>> children = getChildrenAsList();

  if (newChild != nullptr)
    children.push_back(sPtr<Hittable>(newChild));

  return new BVHNode(children, 0, children.size(), 0.0f, 1.0f);
}

rt::Hittable *rt::BVHNode::removeChild(sPtr<Hittable> childToRemove) {
  auto children = getChildrenAsList();

  auto const [eraseStart, _] = std::ranges::remove(children, childToRemove);
  children.erase(eraseStart);

  return new BVHNode(children, 0, children.size(), 0.0f, 1.0f);
}

bool rt::BVHNode::isLeaf() const { return left == nullptr && right == nullptr; }
