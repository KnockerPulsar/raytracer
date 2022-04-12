#pragma once

#include "../HittableList.h"
#include "../Ray.h"
#include "../data_structures/vec3.h"
#include "../materials/MaterialFactory.h"
#include "AARect.h"
#include <memory>

using std::make_shared;

namespace rt {
  class Box : public Hittable {

  public:
    vec3           boxMin, boxMax;
    sPtr<Material> material;
    HittableList   sides;

    Box() = default;
    Box(const vec3 &p0, const vec3 &p1, std::shared_ptr<Material> mat) {
      Create(p0, p1, mat);
    }

    // To work around not being able to use constructors with references
    void Create(const vec3 &p0, const vec3 &p1, std::shared_ptr<Material> mat) {
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

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      outputBox = AABB(boxMin, boxMax);
      return true;
    }

    virtual json GetJson() const override {
      return json{{"type", "box"},
                  {"pos", (boxMin + boxMax) / 2},
                  {"extents", boxMax - boxMin},
                  {"material", material->GetJson()}};
    }

    bool Hit(const Ray &r, float t_min, float t_max,
             HitRecord &rec) const override {
      return sides.Hit(r, t_min, t_max, rec);
    }
  };

  // This is how you use `json.get<Box>()`
  inline void from_json(const json &objectJson, Box &b) {
    auto center  = objectJson["pos"].get<vec3>();
    auto extents = objectJson["extents"].get<vec3>();

    auto min = center - extents / 2;
    auto max = center + extents / 2;
    auto mat = MaterialFactory::FromJson(objectJson["material"]);

    b.Create(min, max, mat);
  }

  inline void to_json(json &j, const Box &b) { j = b.GetJson(); }
} // namespace rt