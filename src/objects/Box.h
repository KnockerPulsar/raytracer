#pragma once

#include "../HittableList.h"
#include "../Ray.h"
#include "../data_structures/vec3.h"
#include "../materials/MaterialFactory.h"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

namespace rt {
  class Box : public Hittable {

  public:
    vec3         boxMin, boxMax;
    HittableList sides;

    Box() : Hittable("Box") {}
    Box(const vec3 &p0, const vec3 &p1);
    Box(float edgeLength);
    Box(const vec3& extents);

    // To work around not being able to use constructors with references
    void Create(const vec3 &p0, const vec3 &p1);

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    virtual json toJsonSpecific() const override;

    bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual void Rasterize(vec3 color) override;

    virtual void changeMaterial(sPtr<Material>& newMat) override;
  };

  // This is how you use `json.get<Box>()`
  // Box must be default constructable too
  inline void from_json(const json &objectJson, Box &b) {
    b.transformation = objectJson["transform"].get<Transformation>();

    auto center  = vec3::Zero();
    auto extents = objectJson["extents"].get<vec3>();

    auto min = center - extents / 2;
    auto max = center + extents / 2;
    auto mat = MaterialFactory::FromJson(objectJson["material"]);

    b.name = objectJson["name"].get<std::string>();

    b.Create(min, max);
    b.changeMaterial(mat);
  }

  inline void to_json(json &j, const Box &b) { j = b.toJson(); }
} // namespace rt
