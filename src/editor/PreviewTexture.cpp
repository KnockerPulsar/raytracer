#include "PreviewTexture.h"
#include <algorithm>

rt::PreviewTexture::PreviewTexture(::Texture texture, int availableWidth, int availableHeight)
    : texture(texture),
      previewWidth(FitIntoArea(ImVec2(availableWidth, availableHeight), ImVec2(texture.width, texture.height)).x),
      previewHeight(FitIntoArea(ImVec2(availableWidth, availableHeight), ImVec2(texture.width, texture.height)).y) {}

rt::PreviewTexture::~PreviewTexture() { UnloadTexture(texture); }

auto rt::PreviewTexture::GetImTextureID() const -> ImTextureID {
  return reinterpret_cast<ImTextureID>(&texture.id);
}

auto rt::PreviewTexture::FitIntoArea(ImVec2 availableArea, ImVec2 textureSize) -> ImVec2 {

  auto const scale = std::min(availableArea.x / textureSize.x, availableArea.y / textureSize.y);
  return {textureSize.x * scale, textureSize.y * scale};
}

auto rt::PreviewTexture::ShouldResize(ImVec2 availableArea) const -> bool {
  auto const newPreviewSize = FitIntoArea(availableArea, ImVec2(texture.width, texture.height));
  return previewWidth != static_cast<int>(newPreviewSize.x) || previewHeight != static_cast<int>(newPreviewSize.y);
}
