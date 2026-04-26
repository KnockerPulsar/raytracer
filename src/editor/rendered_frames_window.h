#pragma once

#include "raytracer/raytracer.h"
#include <optional>

namespace rt {
  namespace editor {

    class RenderedFramesWindow {
    public:
      void draw();
      void onFrameRendered(raytracer::Frame const &frame);

    private:
      std::optional<int> selectedItem;
      std::vector<raytracer::Frame> renderedFrames;
    };

  } // namespace editor
} // namespace rt
