#pragma once

#include "background.h"

namespace rt {
  namespace editor {
    class SkysphereBackground : public Background {
      public:
        SkysphereBackground(std::filesystem::path const& texturePath);
        ~SkysphereBackground();

        bool OnImgui() override;
        void drawBackground(vec3 lookFrom) const override;

        auto toRaytracerBackground() const -> std::unique_ptr<raytracer::Background> override;

      private:
        ::Texture2D skysphereeTexture;
        ::Model skysphereModel;

    };
  } // namespace editor
} // namespace rt
