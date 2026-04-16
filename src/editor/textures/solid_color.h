#pragma once

#include "texture.h"

namespace rt {
  namespace raytracer {
    class Texture;
  }

  namespace editor {
    class SolidColor : public Texture {
    public:
      SolidColor(vec3 color);
      virtual bool OnImgui() override;

      auto createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> override;

    private:
      vec3 color;
    };
  } // namespace editor
} // namespace rt
