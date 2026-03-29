#pragma once
#include "common/Hittable.h"
#include "Triangle.h"

#include <raylib.h>

namespace {
  auto compute_corners(float w, float h) {
    vec3 width  = vec3(w, 0, 0);
    vec3 height = vec3(0, 0, h);

    vec3 botLPoint = -width / 2 - height / 2;
    vec3 topRPoint = width / 2 + height / 2;
    vec3 topLPoint = -width / 2 + height / 2;
    vec3 botRPoint = width / 2 - height / 2;

    /*
     The x axis is flipped in the triangle test scene
     Top view

                    z-
                    |
                    |
                    |
                    |
        x+ --------+---------- x-
                    |
                    |
                    |
                    |
                    z+
    */

    using vert = rt::raytracer::vert;
    vert botL  = vert(botLPoint, vec3(1, 0, 0));
    vert topR  = vert(topRPoint, vec3(0, 1, 0));
    vert topL  = vert(topLPoint, vec3(1, 1, 0));
    vert botR  = vert(botRPoint, vec3(0, 0, 0));

    return std::tuple{botL, topR, topL, botR};
  }
}

namespace rt::raytracer {
  // Base case is an XZ plane
  class Plane : public rt::Hittable {
  public:
    Triangle const t0, t1;

    //  _______
    // |      /|
    // | t0  / |
    // |   c   |
    // |  / t1 |
    // |/______|

    Plane() /*: Hittable("Plane")*/ {}
    Plane(float w, float h)
        /*: Hittable("Plane")*/
        : t0{[&] {
            auto [botL, topR, topL, botR] = compute_corners(w, h);
            return Triangle(botL, topR, botR);
          }()},
          t1{[&] {
            auto [botL, topR, topL, botR] = compute_corners(w, h);
            return Triangle(botL, topL, topR);
          }()} {}

    virtual bool Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override {
      if (t0.Hit(r, t_min, t_max, rec) || t1.Hit(r, t_min, t_max, rec)) {
        rec.closestHit = (Hittable *)this;
        return true;
      }
      return false;
    }

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override {
      outputBox = transformation.regenAABB(AABB(
          std::vector<vec3>{this->t0.v0.p, this->t0.v1.p, this->t0.v2.p, this->t1.v0.p, this->t1.v1.p, this->t1.v2.p}));
      return true;
    }

  };
} // namespace rt
