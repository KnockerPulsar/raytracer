#pragma once

#include "AABB.h"
#include "Constants.h"
#include "Defs.h"
#include "Ray.h"
#include "Transformation.h"
#include "Util.h"
#include "data_structures/vec3.h"
#include <cmath>
#include <memory>
#include <vector>
using std::shared_ptr;

namespace rt {
  class Material;

  struct HitRecord {
    vec3                 p;
    vec3                 normal;
    shared_ptr<Material> mat_ptr;
    float                t, u, v;
    bool                 front_face;

    inline void set_face_normal(const Ray &r, const vec3 &outward_normal) {
      front_face = Vector3DotProduct(outward_normal, r.direction) < 0;
      normal     = front_face ? outward_normal : outward_normal * -1;
    }
  };

  class Hittable {
  public:
    Transformation transformation;

    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const = 0;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const = 0;

    virtual json GetJson() const { return json{{"type", "unimplemented"}}; }

    virtual bool HitTransformed(const Ray &r, float t_min, float t_max,
                                HitRecord &rec) {
      Ray transformedRay = r;

      // Apply inverse transformations in reverse
      transformedRay.origin    = transformation.Inverse(r.origin);
      transformedRay.direction = transformation.Inverse(r.direction);

      if (!this->Hit(transformedRay, t_min, t_max, rec))
        return false;

      rec.p = transformation.Apply(rec.p);
      rec.set_face_normal(transformedRay, transformation.Apply(rec.normal));

      return true;
    }
  };

  class Translate : public Hittable {
  public:
    shared_ptr<Hittable> ptr;
    vec3                 offset;

    Translate(shared_ptr<Hittable> p, const vec3 &dis) : ptr(p), offset(dis) {}
    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const override {
      Ray moved = Ray(r.origin - offset, r.direction, r.time);

      if (!ptr->Hit(moved, t_min, t_max, rec))
        return false;

      rec.p += offset;
      rec.set_face_normal(moved, rec.normal);
      return true;
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      if (!ptr->BoundingBox(t0, t1, outputBox))
        return false;

      outputBox = AABB(outputBox.min + offset, outputBox.max + offset);
      return true;
    }
  };

  class RotateY : public Hittable {
  public:
    shared_ptr<Hittable> ptr;
    float                sinTheta, cosTheta;
    bool                 hasBox;
    AABB                 bBox;

    // Since we use this same function with the signs of the sin flipped
    // (for inverse rotation)
    // I added the `opposite` parameter to deal with that.

    // set as true for
    // x = cos*x + sin*z
    // z = -sin*x + cos*z

    // set as false for
    // x = cos*x - sin*z
    // z = sin*x + cos*z

    inline vec3 Rotate(vec3 in, bool opposite = false) const {
      vec3 out = in;

      // Perhaps lerping instead of branching would be faster???
      int sinSign = opposite * -1 + (1 - opposite) * 1;

      out.x = cosTheta * in.x - sinSign * sinTheta * in.z;
      out.z = sinSign * sinTheta * in.x + cosTheta * in.z;
      return out;
    }

    RotateY(shared_ptr<Hittable> p, float angle) : ptr(p) {
      float rad = DegressToRadians(angle);
      sinTheta  = sin(rad);
      cosTheta  = cos(rad);
      hasBox    = ptr->BoundingBox(0, 1, bBox);

      vec3 min(infinity, infinity, infinity);
      vec3 max(-infinity, -infinity, -infinity);

      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          for (int k = 0; k < 2; k++) {
            float x = i * bBox.max.x - (1 - i) * bBox.min.x;
            float y = j * bBox.max.y - (1 - j) * bBox.min.y;
            float z = k * bBox.max.z - (1 - k) * bBox.min.z;

            auto [newX, _, newZ] = Rotate(vec3(x, y, z), true);

            vec3 tester(newX, y, newZ);

            min.x = fmin(min.x, tester.x);
            max.x = fmax(max.x, tester.x);

            min.y = fmin(min.y, tester.y);
            max.y = fmax(max.y, tester.y);

            min.z = fmin(min.z, tester.z);
            max.z = fmax(max.z, tester.z);
          }
        }
      }
      bBox = AABB(min, max);
    }

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      outputBox = bBox;
      return hasBox;
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const override {
      vec3 origin = Rotate(r.origin);
      vec3 dir    = Rotate(r.direction);

      Ray rotatedR(origin, dir, r.time);

      if (!ptr->Hit(rotatedR, t_min, t_max, rec))
        return false;

      rec.p = Rotate(rec.p, true);
      rec.set_face_normal(rotatedR, Rotate(rec.normal, true));

      return true;
    }
  };
} // namespace rt
