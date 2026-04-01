#pragma once

#include "object.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class Sphere : public Object {
  public:
    Sphere(float radius);
    virtual std::optional<float> hit(const Ray &worldspaceRay) const override;
    virtual void RasterizeWithoutTransformation(vec3 color) override;

  private:
    float radius;
  };
}
