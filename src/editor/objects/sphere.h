#pragma once

#include "editor/IRasterizable.h"
#include "editor/hittable.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class Sphere : public IRasterizable, public Hittable {
  public:
    Sphere(float radius);

    virtual void Rasterize(vec3 color) override {
      // TODO
    }
  };
}
