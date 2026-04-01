#pragma once

#include "common/HittableList.h"
#include "common/Ray.h"
#include "data_structures/vec3.h"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

namespace rt::raytracer {
  class Box : public rt::Hittable {
  public:
    static auto createBox(vec3 p0, vec3 p1) -> rt::raytracer::HittableList;

    Box(float edgeLength);
    Box(const vec3& extents);

    bool Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const override;
    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

  private:
    vec3 const extents;
  };

} // namespace rt
