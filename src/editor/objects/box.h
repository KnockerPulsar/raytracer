#pragma once

#include "editor/IRasterizable.h"
#include "editor/hittable.h"

namespace rt::editor {
  class Box : public IRasterizable, public Hittable {
  public:
    Box(float edgeLength);
    Box(const vec3& extents);
    virtual void Rasterize(vec3 color) override;
    // virtual json toJsonSpecific() const override;
  };

  // // This is how you use `json.get<Box>()`
  // // Box must be default constructable too
  // inline void from_json(const json &objectJson, Box &b) {
  //   b.transformation = objectJson["transform"].get<Transformation>();
  //
  //   auto center  = vec3::Zero();
  //   auto extents = objectJson["extents"].get<vec3>();
  //
  //   auto min = center - extents / 2;
  //   auto max = center + extents / 2;
  //   auto mat = MaterialFactory::FromJson(objectJson["material"]);
  //
  //   b.name = objectJson["name"].get<std::string>();
  //
  //   b = Box(min, max);
  //   b.changeMaterial(mat);
  // }
  //
  // inline void to_json(json &j, const Box &b) { j = b.toJson(); }
} // namespace rt::editor
