#pragma once
#include "AABB.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"
#include "Util.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

namespace rt {
  class BVHNode : public Hittable {
  public:
    std::shared_ptr<Hittable> left = nullptr, right = nullptr;
    AABB                      box;

    BVHNode() = default;

    BVHNode(const std::vector<sPtr<Hittable>> &list, float t0, float t1) : BVHNode(list, 0, list.size(), t0, t1) {}

    BVHNode(const HittableList &list, float t0, float t1) : BVHNode(list.objects, 0, list.objects.size(), t0, t1) {}

    BVHNode(const std::vector<shared_ptr<Hittable>> &srcObjects, size_t start, size_t end, float t0, float t1) {

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

      AABB boxLeft;
      AABB boxRight;

      if (!left->BoundingBoxTransformed(t0, t1, boxLeft) || !right->BoundingBoxTransformed(t0, t1, boxRight))
        std::cerr << "No bounding box in BVHNode constructor.\n";

      box = AABB::SurroundingBox(boxLeft, boxRight);
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override {
      if (!box.Hit(r, t_min, t_max))
        return false;

      bool leftHit  = left->HitTransformed(r, t_min, t_max, rec);
      bool rightHit = right->HitTransformed(r, t_min, leftHit ? rec.t : t_max, rec);

      // if(leftHit || rightHit)
      //   std::cout << "AAAA";
      return leftHit || rightHit;
    }

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override {
      outputBox = box;
      return true;
    }

    static inline bool BoxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis) {
      AABB boxA;
      AABB boxB;

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

    static bool boxXCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) { return BoxCompare(a, b, 0); }

    static bool boxYCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) { return BoxCompare(a, b, 1); }

    static bool boxZCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) { return BoxCompare(a, b, 2); }

    // Using shared pointers causes double free errors
    std::vector<sPtr<Hittable>> getChildrenAsList() override {
      std::vector<sPtr<Hittable>> leftChildren;
      std::vector<sPtr<Hittable>> rightChildren;

      if (auto leftBVH = std::dynamic_pointer_cast<BVHNode>(left); leftBVH == nullptr && left != nullptr)
        leftChildren.push_back(left);
      else {
        leftChildren = left->getChildrenAsList();
        leftChildren.push_back(left);
      }

      if (auto rightBVH = std::dynamic_pointer_cast<BVHNode>(right); rightBVH == nullptr && right != nullptr)
        rightChildren.push_back(right);
      else {
        rightChildren = right->getChildrenAsList();
        rightChildren.push_back(right);
      }

      if (!(leftChildren.size() == 1 && rightChildren.size() == 1 && leftChildren[0] == rightChildren[0]))
        leftChildren.insert(leftChildren.end(), rightChildren.begin(), rightChildren.end());

      return leftChildren;
    }

    std::vector<AABB> getChildrenAABBs() override {
      std::vector<sPtr<Hittable>> children = getChildrenAsList();
      // This is destroyed on function return causing a double

      std::vector<AABB> childrenAABBs;

      for (auto &&e : children) {
        AABB output;
        if (e->BoundingBoxTransformed(0, 1, output))
          childrenAABBs.push_back(output);
      }

      return childrenAABBs;
    }

    // Use with newChild = nullptr to regenerate the tree
    Hittable *addChild(sPtr<Hittable> newChild) override {
      std::vector<sPtr<Hittable>> children = getChildrenAsList();
      std::vector<sPtr<Hittable>> sPtrs    = std::vector<sPtr<Hittable>>();
      sPtrs.reserve(children.size());

      std::for_each(children.begin(), children.end(), [&](sPtr<Hittable> e) { sPtrs.push_back(sPtr<Hittable>(e)); });

      if (newChild != nullptr)
        sPtrs.push_back(sPtr<Hittable>(newChild));

      return new BVHNode(sPtrs, 0, sPtrs.size(), 0.0f, 1.0f);
    }

    bool isLeaf() const { return left == nullptr && right == nullptr; }
  };

} // namespace rt