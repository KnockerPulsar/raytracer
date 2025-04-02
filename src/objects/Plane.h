#pragma once
#include "../Hittable.h"
#include "Sphere.h"
#include "Triangle.h"
#include <raylib.h>

namespace rt {
  // Base case is an XZ plane
  class Plane : public Hittable {
  public:
    Triangle t0, t1;
    float    w, h;

    //  _______
    // |      /|
    // | t0  / |
    // |   c   |
    // |  / t1 |
    // |/______|

    Plane() : Hittable("Plane") {}
    Plane(float w, float h) : Hittable("Plane") { Create(w, h); }

    void Create(float w, float h) {
      this->w = w;
      this->h = h;

      vec3 width     = vec3(w, 0, 0);
      vec3 height    = vec3(0, 0, h);

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

      vert botL = vert(botLPoint, vec3(1, 0, 0));
      vert topR = vert(topRPoint, vec3(0, 1, 0));
      vert topL = vert(topLPoint, vec3(1, 1, 0));
      vert botR = vert(botRPoint, vec3(0, 0, 0));

      t0 = Triangle(botL, topR, botR);
      t1 = Triangle(botL, topL, topR);
    }

    static Plane YZPlane(float w, float h) {
      Plane plane                 = Plane(w, h);
      plane.transformation.setRotation(vec3(0.0f, 0.0f, 90.0f));

      return plane;
    }

    static Plane XYPlane(float w, float h) {
      Plane plane                 = Plane(w, h);
      plane.transformation.setRotation(vec3(90.0f, 0.0f, 0.0f));

      return plane;
    }

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override {
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

    virtual void Rasterize(vec3 color) override {
      static const Color colors[] = {GREEN, BLUE, RED, ORANGE, MAGENTA};
      rlDisableBackfaceCulling();
      //                                 Get "unique" index based on address
      //                                                vvvvvvvvvvvvvvvvvvvv
      // DrawPlane({0, 0, 0}, {w, h}, colors[(long)this >> 4 & 0b11]);
      // All addresses seem to be aligned on 4's so need to shift down a few bits to get
      // different numbers
      // The & with 0b11 is to limit the range to 0->3 to not overflow

      t0.Rasterize(color);
      t1.Rasterize(color);

      rlEnableBackfaceCulling();
    }

    json toJsonSpecific() const override {
      return {
          {"type", "plane"},
          {"width", w},
          {"height", h},
      };
    }

    void changeMaterial(sPtr<Material> &newMat) override {
      this->material = newMat;
      t0.changeMaterial(newMat);
      t1.changeMaterial(newMat);
    }

  };

  inline void from_json(const json &j, Plane &p) {
    p.transformation = j["transform"].get<Transformation>();

    auto w   = j["width"].get<float>();
    auto h   = j["height"].get<float>();
    auto mat = MaterialFactory::FromJson(j["material"]);

    p.Create(w, h);
    p.changeMaterial(mat);

    p.name = j["name"].get<std::string>();
  }

  inline void to_json(json &j, const Plane &p) { j = p.toJson(); }

} // namespace rt
