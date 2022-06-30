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

    json toJsonSpecific() const override;

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;
  };

  class XZRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     x0, x1, z0, z1, y;

    XZRect() = default;
    XZRect(float _x0, float _x1, float _z0, float _z1, float _y, shared_ptr<Material> mat);

    json toJsonSpecific() const override;

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;
  };

  class YZRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     y0, y1, z0, z1, x;

    YZRect() = default;
    YZRect(float _y0, float _y1, float _z0, float _z1, float _x, shared_ptr<Material> mat);

    json toJsonSpecific() const override;

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override;
  };

  inline void to_json(json &j, const XYRect &xy) { j = xy.toJson(); }
  inline void from_json(const json &j, XYRect &plane) {
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x0             = min.x;
    plane.x1             = max.x;
    plane.y0             = min.y;
    plane.y1             = max.y;
    plane.z              = center.z;
    plane.transformation = j["transform"].get<Transformation>();
  }

  inline void to_json(json &j, const XZRect &xz) { j = xz.toJson(); }
  inline void from_json(const json &j, XZRect &plane) {
    
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x0             = min.x;
    plane.x1             = max.x;
    plane.z0             = min.z;
    plane.z1             = max.z;
    plane.y              = center.y;
    plane.transformation = j["transform"].get<Transformation>();
  }

  inline void to_json(json &j, const YZRect &yz) { j = yz.toJson(); }
  inline void from_json(const json &j, YZRect &plane) {
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x              = center.x;
    plane.z0             = min.z;
    plane.z1             = max.z;
    plane.y0             = min.y;
    plane.y1             = max.y;
    plane.transformation = j["transform"].get<Transformation>();
  }
} // namespace rt