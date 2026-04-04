#include "AABB.h"
#include "Ray.h"

#include "common/Constants.h"
#include "common/Transformation.h"

namespace rt {
  AABB::AABB(const vec3 &a, const vec3 &b) {

    min = a;
    max = b;

    Pad();
  }

  AABB::AABB(std::vector<vec3> points) {
    min = vec3(rt::constants::infinity, rt::constants::infinity, rt::constants::infinity);

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

  AABB AABB::ToWorldspace(const AABB &aabb, const Transformation &transform) {
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

} // namespace rt
