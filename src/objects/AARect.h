#pragma once

#include "../../vendor/nlohmann-json/json.hpp"
#include "../AABB.h"
#include "../Defs.h"
#include "../Hittable.h"
#include "../data_structures/vec3.h"
#include "../materials/MaterialFactory.h"
#include <memory>

namespace rt {
  class XYRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     x0, x1, y0, y1, z;

    XYRect() = default;
    XYRect(float _x0, float _x1, float _y0, float _y1, float _z,
           shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), z(_z), mp(mat) {}

    json GetJsonDerived() const override {
      vec3 center  = vec3((x0 + x1) / 2, (y0 + y1) / 2, z);
      vec3 extents = vec3((x1 - x0), (y1 - y0), 0);
      return json{
          {"type", "xy_rect"},
          {"pos", center},
          {"extents", extents},
          {"material", mp->GetJson()},
      };
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const override {
      float t = (z - r.origin.z) / r.direction.z;

      if (t < t_min || t > t_max)
        return false;

      auto [x, y, _] = r.At(t);

      if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;

      rec.u = (x - x0) / (x - x1);
      rec.v = (y - y0) / (y - y1);
      rec.t = t;

      vec3 outwardNormal = vec3(0, 0, 1);
      rec.set_face_normal(r, outwardNormal);
      rec.mat_ptr = mp;
      rec.p       = vec3(x, y, z);
      return true;
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      // Add some padding to the thin dimension to prevent issues
      outputBox = AABB(vec3(x0, y0, z - 1e-4), vec3(x1, y1, z + 1e-4));
      return true;
    }
  };

  class XZRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     x0, x1, z0, z1, y;

    XZRect() = default;
    XZRect(float _x0, float _x1, float _y0, float _y1, float _y,
           shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), z0(_y0), z1(_y1), y(_y), mp(mat) {}

    json GetJsonDerived() const override {
      vec3 center  = vec3((x0 + x1) / 2, y, (z0 + z1) / 2);
      vec3 extents = vec3((x1 - x0), 0, (z1 - z0));
      return json{
          {"type", "xz_rect"},
          {"pos", center},
          {"extents", extents},
          {"material", mp->GetJson()},
      };
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const override {
      float t = (y - r.origin.y) / r.direction.y;

      if (t < t_min || t > t_max)
        return false;

      auto [x, _, z] = r.At(t);

      if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;

      rec.u = (x - x0) / (x - x1);
      rec.v = (z - z0) / (z - z1);
      rec.t = t;

      vec3 outwardNormal = vec3(0, 1, 0);
      rec.set_face_normal(r, outwardNormal);
      rec.mat_ptr = mp;
      rec.p       = vec3(x, y, z);
      return true;
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      // Add some padding to the thin dimension to prevent issues
      outputBox = AABB(vec3(x0, y - 1e-4, z0), vec3(x1, y + 1e-4, z1));
      return true;
    }
  };

  class YZRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     y0, y1, z0, z1, x;

    YZRect() = default;
    YZRect(float _y0, float _y1, float _z0, float _z1, float _x,
           shared_ptr<Material> mat)
        : y0(_y0), y1(_y1), z0(_z0), z1(_z1), x(_x), mp(mat) {}

    json GetJsonDerived() const override {
      vec3 center  = vec3(x, (y0 + y1) / 2, (z0 + z1) / 2);
      vec3 extents = vec3(0, (y1 - y0), (z1 - z0));
      return json{
          {"type", "yz_rect"},
          {"pos", center},
          {"extents", extents},
          {"material", mp->GetJson()},
      };
    }
    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const override {
      float t = (x - r.origin.x) / r.direction.x;

      if (t < t_min || t > t_max)
        return false;

      auto [_, y, z] = r.At(t);

      if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;

      rec.u = (y - y0) / (y - y1);
      rec.v = (z - z0) / (z - z1);
      rec.t = t;

      vec3 outwardNormal = vec3(1, 0, 0);
      rec.set_face_normal(r, outwardNormal);
      rec.mat_ptr = mp;
      rec.p       = vec3(x, y, z);
      return true;
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      // Add some padding to the thin dimension to prevent issues
      outputBox = AABB(vec3(x - 1e-4, y0, z0), vec3(x + 1e-4, y1, z1));
      return true;
    }
  };

  void to_json(json &j, const XYRect &xy) { j = xy.GetJson(); }
  void from_json(const json &j, XYRect &plane) {
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x0 = min.x;
    plane.x1 = max.x;
    plane.y0 = min.y;
    plane.y1 = max.y;
    plane.z  = center.z;
    plane.transformation = j["transform"].get<Transformation>();
  }

  void to_json(json &j, const XZRect &xz) { j = xz.GetJson(); }
  void from_json(const json &j, XZRect &plane) {
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x0 = min.x;
    plane.x1 = max.x;
    plane.z0 = min.z;
    plane.z1 = max.z;
    plane.y  = center.y;
    plane.transformation = j["transform"].get<Transformation>();
  }
  void to_json(json &j, const YZRect &yz) { j = yz.GetJson(); }
  void from_json(const json &j, YZRect &plane) {
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x  = center.x;
    plane.z0 = min.z;
    plane.z1 = max.z;
    plane.y0 = min.y;
    plane.y1 = max.y;
    plane.transformation = j["transform"].get<Transformation>();
  }
} // namespace rt