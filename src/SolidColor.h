#pragma once
#include "Clr.h"
#include "Texture.h"
#include "Vec3.h"
#include <raylib.h>

namespace raytracer {
  class SolidColor : public Texture {
  public:
    SolidColor() = default;
    SolidColor(Vec3 c) : colVal(c) {}

    SolidColor(float r, float g, float b) : colVal(r,g,b) {}

    virtual Vec3 Value(float u, float v, const Vec3 &p) const override { return colVal; }

  private:
    Vec3 colVal;
  };
} // namespace raytracer