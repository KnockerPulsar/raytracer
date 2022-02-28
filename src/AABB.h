#pragma once

#include "Ray.h"
#include "Vec3.h"
#include <cmath>
#include <tuple>

namespace raytracer {
  using raytracer::Ray;

  class AABB {
  public:
    Vec3 min, max;

    AABB() = default;
    AABB(const Vec3 &a, const Vec3 &b) {
      min = a;
      max = b;
    }

    bool Hit(const Ray &r, float tMin, float tMax) const {
      {
        auto invD = 1.0f / r.direction.x;
        auto t0   = (min.x - r.origin.x) * invD;
        auto t1   = (max.x - r.origin.x) * invD;
        if (invD < 0.0f)
          std::swap(t0, t1);
        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;
        if (tMax <= tMin)
          return false;
      }

      {
        auto invD = 1.0f / r.direction.y;
        auto t0   = (min.y - r.origin.y) * invD;
        auto t1   = (max.y - r.origin.y) * invD;
        if (invD < 0.0f)
          std::swap(t0, t1);
        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;
        if (tMax <= tMin)
          return false;
      }

      {
        auto invD = 1.0f / r.direction.z;
        auto t0   = (min.z - r.origin.z) * invD;
        auto t1   = (max.z - r.origin.z) * invD;
        if (invD < 0.0f)
          std::swap(t0, t1);
        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;
        if (tMax <= tMin)
          return false;
      }

      return true;
    }

    static AABB SurroundingBox(AABB b0, AABB b1) {
      Vec3 smol(fmin(b0.min.x, b1.min.x), fmin(b0.min.y, b1.min.y),
                fmin(b0.min.z, b1.min.z));

      Vec3 big(fmax(b0.max.x, b1.max.x), fmax(b0.max.y, b1.max.y),
               fmax(b0.max.z, b1.max.z));

      return AABB(smol, big);
    }
  };
} // namespace raytracer