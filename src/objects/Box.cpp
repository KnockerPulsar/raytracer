#include "Box.h"
#include "AARect.h"
#include "Plane.h"

#include "../HittableBuilder.h"

namespace
{
auto createBox(vec3 p0, vec3 p1) -> rt::HittableList
{
    auto extents = (p1 - p0);

    auto backCenter  = vec3((p0.x + p1.x) / 2, (p0.y + p1.y) / 2, p0.z);
    auto frontCenter = vec3((p0.x + p1.x) / 2, (p0.y + p1.y) / 2, p1.z);

    auto bottomCenter = vec3((p0.x + p1.x) / 2, p0.y, (p0.z + p1.z) / 2);
    auto topCenter    = vec3((p0.x + p1.x) / 2, p1.y, (p0.z + p1.z) / 2);

    auto leftCenter  = vec3(p0.x, (p0.y + p1.y) / 2, (p0.z + p1.z) / 2);
    auto rightCenter = vec3(p1.x, (p0.y + p1.y) / 2, (p0.z + p1.z) / 2);

    using  rt::Plane;
    using rt::HittableBuilder;

    std::vector<std::shared_ptr<rt::Hittable>> sides = {
        HittableBuilder<Plane>(Plane::XYPlane(extents.x, extents.y))
            .rotate(vec3(180, 0, 0))
            .withTranslation(backCenter),
        HittableBuilder<Plane>(Plane::XYPlane(extents.x, extents.y)).withTranslation(frontCenter),
        HittableBuilder<Plane>(extents.x, extents.z).withTranslation(topCenter),
        HittableBuilder<Plane>(extents.x, extents.z).withTranslation(bottomCenter).withRotation(vec3(180, 0, 0)),
        HittableBuilder<Plane>(Plane::YZPlane(extents.y, extents.z)).withTranslation(leftCenter),
        HittableBuilder<Plane>(Plane::YZPlane(extents.y, extents.z))
            .withTranslation(rightCenter)
            .rotate(vec3(0, 0, 180)),
    };

    return rt::HittableList{std::move(sides)};
}
}

namespace rt {
  Box::Box(const vec3 &p0, const vec3 &p1) : Hittable("Box"), boxMin{p0}, boxMax{p1}, sides{createBox(p0, p1)} {}
  Box::Box(float edgeLength) : Box(vec3(edgeLength)) {}
  Box::Box(const vec3 &extents) : Box(-extents / 2, extents / 2) {}

  bool Box::BoundingBox(float t0, float t1, AABB &outputBox) const {
    outputBox = transformation.regenAABB(AABB(boxMin, boxMax));
    return true;
  }

  json Box::toJsonSpecific() const {
    return json{
        {"type", "box"},
        {"extents", boxMax - boxMin},
    };
  }

  bool Box::Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    if (sides.HitTransformed(r, t_min, t_max, rec)) {
      rec.closestHit = (Hittable *)this;
      return true;
    }

    return false;
  }

  void Box::Rasterize(vec3 color) {
    for (auto &side : sides.getChildrenAsList()) {
      side->RasterizeTransformed(side->transformation, color);
    }
  }

  void Box::changeMaterial(sPtr<Material> &newMat) {
    material = newMat;
    for (auto &side : sides.objects) {
      side->changeMaterial(material);
    }
  }
} // namespace rt
