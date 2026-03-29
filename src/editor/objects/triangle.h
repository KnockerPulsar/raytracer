#pragma once

#include "editor/IRasterizable.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class Triangle : public IRasterizable {
    virtual void Rasterize(vec3 color) override {

      // DrawTriangle3D(v0.p, v1.p, v2.p, color.toRaylibColor(255));
      // auto  normalStart = (v0.p + v1.p + v2.p) / 3;
      // Color inv         = color.toRaylibColor(255);
      // inv =
      //     Color{(unsigned char)(255 - inv.r), (unsigned char)(255 - inv.g), (unsigned char)(255 - inv.b), uint8_t(255)};
      // DrawLine3D(normalStart, normalStart + normal, inv);
    }
  };
}
