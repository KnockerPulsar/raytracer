#pragma once

#include "editor/IRasterizable.h"
#include "editor/hittable.h"

namespace rt::editor {
  class Plane : public IRasterizable, public Hittable {
  public:
    Plane(float w, float h);
    // static Plane YZPlane(float w, float h) {
    //   Plane plane                 = Plane(w, h);
    //   plane.transformation.setRotation(vec3(0.0f, 0.0f, 90.0f));
    //
    //   return plane;
    // }
    //
    // static Plane XYPlane(float w, float h) {
    //   Plane plane                 = Plane(w, h);
    //   plane.transformation.setRotation(vec3(90.0f, 0.0f, 0.0f));
    //
    //   return plane;
    // }

    virtual void Rasterize(vec3 color) override {
      // static const Color colors[] = {GREEN, BLUE, RED, ORANGE, MAGENTA};
      // rlDisableBackfaceCulling();
      // //                                 Get "unique" index based on address
      // //                                                vvvvvvvvvvvvvvvvvvvv
      // // DrawPlane({0, 0, 0}, {w, h}, colors[(long)this >> 4 & 0b11]);
      // // All addresses seem to be aligned on 4's so need to shift down a few bits to get
      // // different numbers
      // // The & with 0b11 is to limit the range to 0->3 to not overflow
      //
      // t0.Rasterize(color);
      // t1.Rasterize(color);
      //
      // rlEnableBackfaceCulling();
    }

    // json toJsonSpecific() const override {
    //   return {
    //       {"type", "plane"},
    //       {"width", w},
    //       {"height", h},
    //   };
    // }

    // void changeMaterial(sPtr<Material> &newMat) override {
    //   this->material = newMat;
    //   t0.changeMaterial(newMat);
    //   t1.changeMaterial(newMat);
    // }
  };

  // inline void from_json(const json &j, Plane &p) {
  //   p.transformation = j["transform"].get<Transformation>();
  //
  //   auto w   = j["width"].get<float>();
  //   auto h   = j["height"].get<float>();
  //   auto mat = MaterialFactory::FromJson(j["material"]);
  //
  //   p.Create(w, h);
  //   p.changeMaterial(mat);
  //
  //   p.name = j["name"].get<std::string>();
  // }
  //
  // inline void to_json(json &j, const Plane &p) { j = p.toJson(); }
}
