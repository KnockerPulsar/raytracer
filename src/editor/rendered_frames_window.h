#pragma once

#include "raytracer/raytracer.h"

#include <optional>
#include <chrono>

namespace rt {
  namespace editor {

    class RenderedFramesWindow {
    public:
      struct Frame {
        raytracer::Frame                                   frame;
        std::chrono::time_point<std::chrono::system_clock> renderFinishTime;
      };

      void draw();
      void onFrameRendered(raytracer::Frame const &frame);

    private:
      std::optional<int> selectedItem;
      std::vector<Frame> renderedFrames;
    };

  } // namespace editor
} // namespace rt
