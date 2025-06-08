#include "PreviewTexture.h"
#include "Utils.h"

rt::PreviewTexture::PreviewTexture(::Texture texture, int availableWidth, int availableHeight)
    : texture(texture), fitArea(EditorUtils::FitIntoArea(ImVec2(availableWidth, availableHeight),
                                                         ImVec2(texture.width, texture.height))) {}

rt::PreviewTexture::~PreviewTexture() { UnloadTexture(texture); }

auto rt::PreviewTexture::GetImTextureID() const -> ImTextureID {
  return reinterpret_cast<ImTextureID>(&texture.id);
}

auto rt::PreviewTexture::ShouldResize(ImVec2 availableArea) const -> bool {
  if(texture.width < minDimensionSize || texture.height < minDimensionSize)
    return true;

  auto const fitArea = EditorUtils::FitIntoArea(availableArea, ImVec2(texture.width, texture.height));
  auto const dx      = std::fabs(this->fitArea.x - fitArea.x);
  auto const dy      = std::fabs(this->fitArea.y - fitArea.y);
  return dx > 1.0 || dy > 1.0f;
}
