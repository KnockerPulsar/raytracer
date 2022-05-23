#pragma once

#include "../../vendor/nlohmann-json/json.hpp"
#include "../Defs.h"
#include "../Hittable.h"
#include "../data_structures/vec3.h"
#include "../materials/MaterialFactory.h"
#include <memory>

namespace rt {

  class Material;

  class XYRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     x0, x1, y0, y1, z;

    XYRect() = default;
    XYRect(float _x0, float _x1, float _y0, float _y1, float _z, shared_ptr<Material> mat);

    json GetJsonDerived() const override;

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;
  };

  class XZRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     x0, x1, z0, z1, y;

    XZRect() = default;
    XZRect(float _x0, float _x1, float _z0, float _z1, float _y, shared_ptr<Material> mat);

    json GetJsonDerived() const override;

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;
  };

  class YZRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     y0, y1, z0, z1, x;

    YZRect() = default;
    YZRect(float _y0, float _y1, float _z0, float _z1, float _x, shared_ptr<Material> mat);

    json GetJsonDerived() const override;

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;
  };
} // namespace rt