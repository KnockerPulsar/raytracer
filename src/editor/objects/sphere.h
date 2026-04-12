#pragma once

#include "object.h"

#include "data_structures/vec3.h"

namespace rt {
  class Raytracable;

  namespace editor {
    class Sphere : public Object {
    public:
      Sphere(float radius);

      virtual std::optional<float> hit(const Ray &worldspaceRay) const override;
      virtual void                 RasterizeWithoutTransformation(vec3 color) override;
      auto                         toRaytracable() const -> std::unique_ptr<rt::Raytracable> override;
      void                         OnImgui() override;

    private:
      float radius;
    };
  } // namespace editor
} // namespace rt
