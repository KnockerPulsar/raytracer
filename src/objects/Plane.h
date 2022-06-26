#pragma once
#include "../Hittable.h"
#include "Triangle.h"
#include <raylib.h>

namespace rt {
  // Base case is an XZ plane
  class Plane : public Hittable {
  public:
    Triangle t0, t1;
    vec3     c;
    float    w, h;

    //  _______
    // |      /|
    // | t0  / |
    // |   c   |
    // |  / t1 |
    // |/______|

    Plane(vec3 c, float w, float h, sPtr<Material> mat) : c(c), w(w), h(h) {
      material = mat;
      vec3 width  = vec3(w, 0, 0);
      vec3 height = vec3(0, 0, h);

      vec3 botLPoint = c - width / 2 - height / 2;
      vec3 topRPoint = c + width / 2 + height / 2;
      vec3 topLPoint = c - width / 2 + height / 2;
      vec3 botRPoint = c + width / 2 - height / 2;

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

      vert botL = vert(botLPoint, vec3(1, 0, 0));
      vert topR = vert(topRPoint, vec3(0, 1, 0));
      vert topL = vert(topLPoint, vec3(1, 1, 0));
      vert botR = vert(botRPoint, vec3(0, 0, 0));

      t0 = Triangle(botL, topR, botR, mat);
      t1 = Triangle(botL, topL, topR, mat);
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override {
      if (t0.Hit(r, t_min, t_max, rec) || t1.Hit(r, t_min, t_max, rec)) {
        rec.closestHit = (Hittable *)this;
        return true;
      }
      return false;
    }

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const override {
      outputBox = transformation.regenAABB(
          AABB(std::vector<vec3>{this->t0.v0.p, this->t0.v1.p, this->t0.v2.p, this->t1.v0.p, this->t1.v1.p, this->t1.v2.p}));
    // Using shared pointers causes double free errors
      return true;
    }

    virtual void Rasterize() override {
      static const Color colors[] = {GREEN, BLUE, RED, ORANGE, MAGENTA};
      rlDisableBackfaceCulling();   
      //                                 Get "unique" index based on address
      //                                                vvvvvvvvvvvvvvvvvvvv
      DrawPlane(c, {w, h}, colors[(long)this>>4 & 0b11]);
      // All addresses seem to be aligned on 4's so need to shift down a few bits to get
      // different numbers
      // The & with 0b11 is to limit the range to 0->3 to not overflow

      rlEnableBackfaceCulling();
    }
  };
} // namespace rt