#pragma once

#include "object.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class Sphere : public Object {
  public:
    Sphere(float radius);
    virtual void Rasterize(vec3 color) override;

  private:
    float radius;
  };
}
