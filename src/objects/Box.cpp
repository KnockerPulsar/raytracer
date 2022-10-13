#include "Box.h"
#include "AARect.h"
#include "Plane.h"

#include "../HittableBuilder.h"

namespace rt {
  Box::Box(const vec3 &p0, const vec3 &p1) { Create(p0, p1); }
  
  Box::Box(float edgeLength) : Box(vec3(edgeLength)) {}
  Box::Box(const vec3 &extents) { Create(-extents / 2, extents / 2); }


  void Box::Create(const vec3 &p0, const vec3 &p1) {
    boxMin = p0;
    boxMax = p1;

    auto extents = (p1 - p0);

    auto backCenter  = vec3((p0.x + p1.x) / 2, (p0.y + p1.y) / 2, p0.z);
    auto frontCenter = vec3((p0.x + p1.x) / 2, (p0.y + p1.y) / 2, p1.z);

    auto bottomCenter = vec3((p0.x + p1.x) / 2, p0.y, (p0.z + p1.z) / 2);
    auto topCenter    = vec3((p0.x + p1.x) / 2, p1.y, (p0.z + p1.z) / 2);

    auto leftCenter  = vec3(p0.x, (p0.y + p1.y) / 2, (p0.z + p1.z) / 2);
    auto rightCenter = vec3(p1.x, (p0.y + p1.y) / 2, (p0.z + p1.z) / 2);

    sides.Add(HittableBuilder<Plane>(Plane::XYPlane(extents.x, extents.y))
                  .rotate(vec3(180, 0, 0))
                  .withTranslation(backCenter)
                  .build());

    sides.Add(HittableBuilder<Plane>(Plane::XYPlane(extents.x, extents.y)).withTranslation(frontCenter).build());

    sides.Add(HittableBuilder<Plane>(extents.x, extents.z).withTranslation(topCenter).build());

    sides.Add(
        HittableBuilder<Plane>(extents.x, extents.z).withTranslation(bottomCenter).withRotation(vec3(180, 0, 0)).build()
    );

    sides.Add(HittableBuilder<Plane>(Plane::YZPlane(extents.y, extents.z)).withTranslation(leftCenter).build());

    sides.Add(HittableBuilder<Plane>(Plane::YZPlane(extents.y, extents.z))
                  .withTranslation(rightCenter)
                  .rotate(vec3(0, 0, 180))
                  .build());
  }

  bool Box::BoundingBox(float t0, float t1, AABB &outputBox) {
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