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

    static auto FitIntoArea(ImVec2 availableArea, ImVec2 textureSize) -> ImVec2;

    auto ShouldResize(ImVec2 availableArea) const -> bool;

    int const previewWidth, previewHeight;

  private:
    Texture2D const texture;
  };
} // namespace rt::Editor
