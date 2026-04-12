#pragma once

#include "object.h"

namespace rt {
  namespace raytracer { class Model; }
  namespace editor {
    class Mesh : public Object {
    public:
      Mesh(std::filesystem::path path);
      void                 RasterizeWithoutTransformation(vec3 color) override;
      std::optional<float> hit(const Ray &worldspaceRay) const override;
      auto                 toRaytracable() const -> std::unique_ptr<rt::Raytracable> override;
      void                 OnImgui() override;

    private:
      std::string filepathStr;
      std::shared_ptr<::Model> model;
    };
  } // namespace editor
} // namespace rt
