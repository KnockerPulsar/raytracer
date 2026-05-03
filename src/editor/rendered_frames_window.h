#pragma once

#include "raytracer/raytracer.h"

#include <cstdint>
#include <set>
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
      void focusOnItem(int itemIndex);
      void eraseSelected();
      void saveSelected(std::filesystem::path directory);
      void selectRange(uint64_t untilItem);
      void toggleSelection(uint64_t item);

      void renderMainLayout();
      void renderSidePanel();
      void renderFrameList();
      void renderFrameListFooter();
      void renderFrameListElement(uint64_t i, float rowHeight);

      // Used for multi-selection
      std::set<int> selectedItems;

      // Determines which list item is focused
      std::optional<int> focusedItem;
      std::vector<Frame> renderedFrames;
    };

  } // namespace editor
} // namespace rt
