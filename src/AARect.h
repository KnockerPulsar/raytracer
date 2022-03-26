#pragma once

#include "AABB.h"
#include "Defs.h"
#include "Hittable.h"
#include "MaterialFactory.h"
#include "Vec3.h"
#include <memory>

namespace raytracer {
  class XYRect : public Hittable {
  public:
    std::shared_ptr<Material> mp;
    float                     x0, x1, y0, y1, z;

    XYRect() = default;
    XYRect(float _x0, float _x1, float _y0, float _y1, float _z,
           shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), z(_z), mp(mat) {}

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

      Vec3 outwardNormal = Vec3(0, 0, 1);
      rec.set_face_normal(r, outwardNormal);
      rec.mat_ptr = mp;
      rec.p       = Vec3(x, y, z);
      return true;
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      // Add some padding to the thin dimension to prevent issues
      outputBox = AABB(Vec3(x0, y0, z - 1e-4), Vec3(x1, y1, z + 1e-4));
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

    XZRect(json objectJson) {
      Vec3 center  = Vec3::FromJson(objectJson["pos"]);
      Vec3 extents = Vec3::FromJson(objectJson["extents"]);
      mp           = MaterialFactory::FromJson(objectJson["material"]);

      Vec3 min = center - extents / 2;
      Vec3 max = center + extents / 2;

      x0 = min.x;
      x1 = max.x;
      z0 = min.z;
      z1 = max.z;
      y  = center.y;
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

      Vec3 outwardNormal = Vec3(0, 1, 0);
      rec.set_face_normal(r, outwardNormal);
      rec.mat_ptr = mp;
      rec.p       = Vec3(x, y, z);
      return true;
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      // Add some padding to the thin dimension to prevent issues
      outputBox = AABB(Vec3(x0, y - 1e-4, z0), Vec3(x1, y + 1e-4, z1));
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

      Vec3 outwardNormal = Vec3(1, 0, 0);
      rec.set_face_normal(r, outwardNormal);
      rec.mat_ptr = mp;
      rec.p       = Vec3(x, y, z);
      return true;
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      // Add some padding to the thin dimension to prevent issues
      outputBox = AABB(Vec3(x - 1e-4, y0, z0), Vec3(x + 1e-4, y1, z1));
      return true;
    }
  };
} // namespace raytracer