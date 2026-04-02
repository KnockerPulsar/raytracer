#pragma once

#include "object.h"

namespace rt::editor {
  class Plane : public Object {
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

    virtual void         RasterizeWithoutTransformation(vec3 color) override;
    std::optional<float> hit(const Ray &worldspaceRay) const override;
    auto                 toRaytracerHittable() const -> std::shared_ptr<rt::Hittable> override;

    // json toJsonSpecific() const override {
    //   return {
    //       {"type", "plane"},
    //       {"width", w},
    //       {"height", h},
    //   };
    // }

  private:
    float w, h;
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
