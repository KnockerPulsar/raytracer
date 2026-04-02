#pragma once

#include "object.h"

namespace rt::editor {
  class Mesh : public Object {
  public:
      Mesh(std::filesystem::path path);
      void RasterizeWithoutTransformation(vec3 color) override;
      std::optional<float> hit(const Ray &worldspaceRay) const override;
      auto toRaytracerHittable() const -> std::shared_ptr<rt::Hittable> override;
    private:
      Model const model;
  };
}
