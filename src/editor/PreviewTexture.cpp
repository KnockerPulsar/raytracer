#include "PreviewTexture.h"
#include "Utils.h"

rt::PreviewTexture::PreviewTexture(::Texture texture, int availableWidth, int availableHeight)
    : texture(texture),
      previewWidth(EditorUtils::FitIntoArea(ImVec2(availableWidth, availableHeight), ImVec2(texture.width, texture.height)).x),
      previewHeight(EditorUtils::FitIntoArea(ImVec2(availableWidth, availableHeight), ImVec2(texture.width, texture.height)).y) {}

rt::PreviewTexture::~PreviewTexture() { UnloadTexture(texture); }

auto rt::PreviewTexture::GetImTextureID() const -> ImTextureID {
  return reinterpret_cast<ImTextureID>(&texture.id);
}

auto rt::PreviewTexture::ShouldResize(ImVec2 availableArea) const -> bool {
  auto const newPreviewSize = EditorUtils::FitIntoArea(availableArea, ImVec2(texture.width, texture.height));
  return previewWidth != static_cast<int>(newPreviewSize.x) || previewHeight != static_cast<int>(newPreviewSize.y);
}
