#pragma once
#include "../Hittable.h"
#include <cmath>
#include <raylib.h>
#include <rlgl.h>
#include <vector>
namespace rt {
  struct vert {
    vec3 p;
    vec3 uvw; // should be vec2

    vert() = default;
    vert(vec3 vec) : p(vec) {}
    vert(vec3 vec, vec3 _uvw) : p(vec), uvw(_uvw) {}
  };

  // Don't really need to rasterize a triangle by iteself YET
  class Triangle : public Hittable {
  public:
    vert v0, v1, v2;
    vec3 normal;

    Triangle() = default;
    Triangle(vert _v0, vert _v1, vert _v2) : v0(_v0), v1(_v1), v2(_v2) {
      vec3 v01 = (v1.p - v0.p).Normalize();
      vec3 v02 = (v2.p - v0.p).Normalize();

      normal = vec3::CrsProd(v01, v02).Normalize();
    }
    Triangle(vec3 p0, vec3 p1, vec3 p2) : v0(p0), v1(p1), v2(p2) {
      vec3 v01 = (v1.p - v0.p).Normalize();
      vec3 v02 = (v2.p - v0.p).Normalize();

      normal = vec3::CrsProd(v01, v02);

      v0.uvw = vec3(0, 0, 0);

      v1.uvw = vec3(1, 0, 0);

      v2.uvw = vec3(0, 1, 0);
    }

    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override {

      vec3  v0v1 = v1.p - v0.p;
      vec3  v0v2 = v2.p - v0.p;
      vec3  pvec = vec3::CrsProd(r.direction, v0v2);
      float det  = vec3::DotProd(v0v1, pvec);

      // ray and triangle are parallel if det is close to 0
      if (fabs(det) < 1e-6)
        return false;

      float invDet = 1 / det;

      vec3  tvec = r.origin - v0.p;
      float u    = vec3::DotProd(tvec, pvec) * invDet;
      if (u < 0 || u > 1)
        return false;

      vec3  qvec = vec3::CrsProd(tvec, v0v1);
      float v    = vec3::DotProd(r.direction, qvec) * invDet;
      if (v < 0 || u + v > 1)
        return false;

      float t = vec3::DotProd(v0v2, qvec) * invDet;

      if (t > t_min && t < t_max) {
        vec3 p     = r.At(t);
        rec.p      = p;
        rec.normal = normal;

        // UV interpolation
        // source:
        // https://www.gamedev.net/forums/topic/513814-raytracing-and-texture-mapping-triangles/
        vec3 properUVs = v0.uvw + (v1.uvw - v0.uvw) * u + (v2.uvw - v0.uvw) * v;

        rec.u       = properUVs.x;
        rec.v       = properUVs.y;
        rec.mat_ptr = material;
        rec.t       = t;
        rec.set_face_normal(r, normal);
        rec.closestHit = (Hittable *)this;

        return true;
      }

      return false;
    }

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override {
      outputBox = transformation.regenAABB(AABB(std::vector<vec3>{{v0.p, v1.p, v2.p}}));
      return true;
    }

    virtual void Rasterize(vec3 color) override {

      // TODO: Figure out a way to order vertices and just draw once
      DrawTriangle3D(v0.p, v1.p, v2.p, color.toRaylibColor(255));
      auto  normalStart = (v0.p + v1.p + v2.p) / 3;
      Color inv         = color.toRaylibColor(255);
      inv =
          Color{(unsigned char)(255 - inv.r), (unsigned char)(255 - inv.g), (unsigned char)(255 - inv.b), uint8_t(255)};
      DrawLine3D(normalStart, normalStart + normal, inv);
    }
  };
} // namespace rt