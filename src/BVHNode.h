#include "AABB.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Util.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

namespace raytracer {
  class BVHNode : public Hittable {
  public:
    std::shared_ptr<Hittable> left, right;
    AABB                      box;

    BVHNode() = default;

    BVHNode(const HittableList &list, float t0, float t1)
        : BVHNode(list.objects, 0, list.objects.size(), t0, t1) {}

    BVHNode(const std::vector<shared_ptr<Hittable>> &srcObjects, size_t start,
            size_t end, float t0, float t1) {
      auto objs = srcObjects;

      int  axis       = RandomInt(0, 2);
      auto comparator = (axis == 0)   ? boxXCompare
                        : (axis == 1) ? boxYCompare
                                      : boxZCompare;

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

      if (!left->BoundingBox(t0, t1, boxLeft) ||
          !right->BoundingBox(t0, t1, boxRight))
        std::cerr << "No bounding box in BVHNode constructor.\n";

      box = AABB::SurroundingBox(boxLeft, boxRight);
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const override {
      if (!box.Hit(r, t_min, t_max))
        return false;

      bool leftHit  = left->Hit(r, t_min, t_max, rec);
      bool rightHit = right->Hit(r, t_min, leftHit ? rec.t : t_max, rec);

      // if(leftHit || rightHit)
      //   std::cout << "AAAA";
      return leftHit || rightHit;
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      outputBox = box;
      return true;
    }

    static inline bool BoxCompare(const shared_ptr<Hittable> a,
                                  const shared_ptr<Hittable> b, int axis) {
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

    static bool boxXCompare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
      return BoxCompare(a, b, 0);
    }

    static bool boxYCompare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
      return BoxCompare(a, b, 1);
    }

    static bool boxZCompare(const shared_ptr<Hittable> a,
                            const shared_ptr<Hittable> b) {
      return BoxCompare(a, b, 2);
    }
  };
} // namespace raytracer