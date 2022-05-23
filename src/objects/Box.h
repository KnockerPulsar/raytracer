#pragma once

#include "../HittableList.h"
#include "../IRasterizable.h"
#include "../Ray.h"
#include "../data_structures/vec3.h"
#include "../materials/MaterialFactory.h"
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

using std::make_shared;

namespace rt {
  class Box : public Hittable {

  public:
    vec3           boxMin, boxMax;
    sPtr<Material> material;
    HittableList   sides;

    Box() = default;
    Box(const vec3 &p0, const vec3 &p1, std::shared_ptr<Material> mat);

    // To work around not being able to use constructors with references
    void Create(const vec3 &p0, const vec3 &p1, std::shared_ptr<Material> mat);

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    virtual json GetJsonDerived() const override;

    bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    void Rasterize() override;
  };

  // This is how you use `json.get<Box>()`
  inline void from_json(const json &objectJson, Box &b) {
    auto center  = objectJson["pos"].get<vec3>();
    auto extents = objectJson["extents"].get<vec3>();

    auto min = center - extents / 2;
    auto max = center + extents / 2;
    auto mat = MaterialFactory::FromJson(objectJson["material"]);

    b.Create(min, max, mat);
    b.transformation = objectJson["transform"].get<Transformation>();
  }

  inline void to_json(json &j, const Box &b) { j = b.GetJson(); }
} // namespace rt