#pragma once

#include "background.h"

namespace rt {
  namespace editor {
    class SolidColorBackground : public Background {
      public:
        SolidColorBackground(vec3 backgroundColor);

        void OnImgui() override;
        void drawBackground(vec3 lookFrom) const override;

        auto toRaytracerBackground() const -> std::unique_ptr<raytracer::Background> override;

      private:
        vec3 backgroundColor;
    };
  } // namespace editor
} // namespace rt
