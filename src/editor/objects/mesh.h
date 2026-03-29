#pragma once

#include "editor/IRasterizable.h"
#include "editor/hittable.h"

namespace rt::editor {
  class Mesh : public IRasterizable, public Hittable {
  public:
      Mesh(std::filesystem::path path);
      void Rasterize(vec3 color) override;
  };
}
