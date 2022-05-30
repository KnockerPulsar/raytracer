#include "Box.h"
#include "AARect.h"

namespace rt {
  Box::Box(const vec3 &p0, const vec3 &p1, std::shared_ptr<Material> mat) { Create(p0, p1, mat); }

  void Box::Create(const vec3 &p0, const vec3 &p1, std::shared_ptr<Material> mat) {
    boxMin   = p0;
    boxMax   = p1;
    material = mat;

    sides.Add(make_shared<XYRect>(p0.x, p1.x, p0.y, p1.y, p1.z, mat));
    sides.Add(make_shared<XYRect>(p0.x, p1.x, p0.y, p1.y, p0.z, mat));

    sides.Add(make_shared<XZRect>(p0.x, p1.x, p0.z, p1.z, p1.y, mat));
    sides.Add(make_shared<XZRect>(p0.x, p1.x, p0.z, p1.z, p0.y, mat));

    sides.Add(make_shared<YZRect>(p0.y, p1.y, p0.z, p1.z, p0.x, mat));
    sides.Add(make_shared<YZRect>(p0.y, p1.y, p0.z, p1.z, p1.x, mat));
  }

  bool Box::BoundingBox(float t0, float t1, AABB &outputBox) const {
    outputBox = transformation.regenAABB(AABB(boxMin, boxMax));
    return true;
  }

  json Box::GetJsonDerived() const {
    return json{{"type", "box"},
                {"pos", (boxMin + boxMax) / 2},
                {"extents", boxMax - boxMin},
                {"material", material->GetJson()}};
  }

  bool Box::Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
    if (sides.Hit(r, t_min, t_max, rec)) {
      rec.closestHit = (Hittable *)this;
      return true;
    }

    return false;
  }

  void Box::Rasterize() {
    auto  extents  = (boxMax - boxMin);
    Color boxColor = GREEN;

    if (auto* diffLight = dynamic_cast<DiffuseLight *>(material.get()); diffLight) {
      vec3 lightColor = diffLight->emitted(0, 0, vec3());
      boxColor        = lightColor.toRaylibColor(255);
    }

    DrawCube(Vector3Zero(), extents.x, extents.y, extents.z, boxColor);
  }

  void Box::OnImgui() {
    Hittable::OnImgui();
    material->OnImgui();
  }
} // namespace rt