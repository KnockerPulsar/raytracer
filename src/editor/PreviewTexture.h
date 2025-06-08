#pragma once

#include <imgui.h>
#include <raylib.h>

namespace rt {
  class PreviewTexture {
  public:
    // Image is wider than it is tall, use the available width as a
    // baseline and scale the height accordingly
    // Otherwise, use the height as the baseline
    PreviewTexture(::Texture texture, int availableWidth, int availableHeight);

    ~PreviewTexture();

    auto GetImTextureID() const -> ImTextureID;
    auto ShouldResize(ImVec2 availableArea) const -> bool;
    auto FitDisplayArea() const -> ImVec2  { return fitArea; }

    auto const inline static minDimensionSize = 32;

  private:
    Texture2D const texture;
    ImVec2 const fitArea;
  };
} // namespace rt::Editor
