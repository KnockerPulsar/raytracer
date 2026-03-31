#include "Box.h"

#include "AARect.h"


namespace rt::raytracer {
  auto Box::createBox(vec3 p0, vec3 p1) -> rt::raytracer::HittableList {
    auto extents = (p1 - p0);

    auto backCenter  = vec3((p0.x + p1.x) / 2, (p0.y + p1.y) / 2, p0.z);
    auto frontCenter = vec3((p0.x + p1.x) / 2, (p0.y + p1.y) / 2, p1.z);

    auto bottomCenter = vec3((p0.x + p1.x) / 2, p0.y, (p0.z + p1.z) / 2);
    auto topCenter    = vec3((p0.x + p1.x) / 2, p1.y, (p0.z + p1.z) / 2);

    auto leftCenter  = vec3(p0.x, (p0.y + p1.y) / 2, (p0.z + p1.z) / 2);
    auto rightCenter = vec3(p1.x, (p0.y + p1.y) / 2, (p0.z + p1.z) / 2);

    // TODO
    std::vector<std::shared_ptr<rt::Hittable>> sides = {
        // HittableBuilder<Plane>(Plane::XYPlane(extents.x, extents.y))
        //     .rotate(vec3(180, 0, 0))
        //     .withTranslation(backCenter),
        //
        // HittableBuilder<Plane>(Plane::XYPlane(extents.x, extents.y)).withTranslation(frontCenter),
        // HittableBuilder<Plane>(extents.x, extents.z).withTranslation(topCenter),
        // HittableBuilder<Plane>(extents.x, extents.z).withTranslation(bottomCenter).withRotation(vec3(180, 0, 0)),
        // HittableBuilder<Plane>(Plane::YZPlane(extents.y, extents.z)).withTranslation(leftCenter),
        // HittableBuilder<Plane>(Plane::YZPlane(extents.y, extents.z))
        //     .withTranslation(rightCenter)
        //     .rotate(vec3(0, 0, 180)),
    };

    return rt::raytracer::HittableList{std::move(sides)};
  }

  Box::Box(const vec3 &p0, const vec3 &p1) : boxMin{p0}, boxMax{p1}, sides{createBox(p0, p1)} {}
  Box::Box(float edgeLength) : Box(vec3(edgeLength)) {}
  Box::Box(const vec3 &extents) : Box(-extents / 2, extents / 2) {}

  bool Box::BoundingBox(float t0, float t1, AABB &outputBox) const {
    outputBox = transformation.regenAABB(AABB(boxMin, boxMax));
    return true;
  }

  bool Box::Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const {
    if (sides.hit(r, t_min, t_max, rec)) {
      rec.closestHit = (Hittable *)this;
      return true;
    }

    return false;
  }
} // namespace rt
