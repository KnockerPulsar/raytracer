#include "AABB.h"
#include "Ray.h"

namespace rt {
  AABB::AABB(const vec3 &a, const vec3 &b) {

    min = a;
    max = b;

    Pad();
  }

  AABB::AABB(std::vector<vec3> points) {
    min = vec3(infinity, infinity, infinity);

    max = -min;
    for (auto &&point : points) {
      min.x = fmin(point.x, min.x);
      min.y = fmin(point.y, min.y);
      min.z = fmin(point.z, min.z);

      max.x = fmax(point.x, max.x);
      max.y = fmax(point.y, max.y);
      max.z = fmax(point.z, max.z);
    }

    Pad();
  }

  bool AABB::Hit(const Ray &r, float tMin, float tMax) const {
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

  AABB AABB::SurroundingBox(AABB b0, AABB b1) {
    vec3 smol(fmin(b0.min.x, b1.min.x), fmin(b0.min.y, b1.min.y), fmin(b0.min.z, b1.min.z));

    vec3 big(fmax(b0.max.x, b1.max.x), fmax(b0.max.y, b1.max.y), fmax(b0.max.z, b1.max.z));

    return AABB(smol, big);
  }

  void AABB::Pad() {
    // In case the points are coplanar
    float eps = 0.01;
    if (abs(min.x - max.x) < eps) {
      min.x -= eps;
      max.x += eps;
    }
    if (abs(min.y - max.y) < eps) {
      min.y -= eps;
      max.y += eps;
    }
    if (abs(min.z - max.z) < eps) {
      min.z -= eps;
      max.z += eps;
    }
  }
} // namespace rt