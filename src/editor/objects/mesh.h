#pragma once

#include "object.h""

namespace rt::editor {
  class Mesh : public Object {
  public:
      Mesh(std::filesystem::path path);
      void Rasterize(vec3 color) override;
  private:
      Model const model;
  };
}
