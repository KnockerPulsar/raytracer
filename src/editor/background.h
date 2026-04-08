#pragma once

#include "editor/IRasterizable.h"
#include "editor/IImguiDrawable.h"

namespace rt {
  namespace raytracer {
    class Background;
  }

  namespace editor {
    class Background : public IImguiDrawable {
    public:
      virtual ~Background() = default;

      virtual void drawBackground(vec3 lookFrom) const = 0;
      virtual auto toRaytracerBackground() const -> std::unique_ptr<raytracer::Background> = 0;
    };
  } // namespace editor
} // namespace rt
