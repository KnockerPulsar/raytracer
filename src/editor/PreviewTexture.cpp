#include "PreviewTexture.h"
#include "textures/Texture.h"

#include "editor/Utils.h"

rt::PreviewTexture::~PreviewTexture() {
  if(texture)
    UnloadTexture(*texture);
}

void rt::PreviewTexture::possiblyGeneratePreview(rt::Texture &tex) {
  auto const regionMax     = ImGui::GetWindowContentRegionMax();
  auto const regionMin     = ImGui::GetWindowContentRegionMin();
  auto const availableArea = ImVec2{regionMax.x - regionMin.x, regionMax.y - regionMin.y};

  if (!texture)
    texture.emplace(tex.generatePreview(availableArea.x, availableArea.y, scale));
}

void rt::PreviewTexture::OnImgui() {
  ImGui::BeginGroupPanel("Texture preview");
  {
    ImGui::Dummy({-1, 10});

    auto const regionMax     = ImGui::GetWindowContentRegionMax();
    auto const regionMin     = ImGui::GetWindowContentRegionMin();
    auto const availableArea = ImVec2{regionMax.x - regionMin.x, regionMax.y - regionMin.y};

    ImGui::Image(GetImTextureID(), FitDisplayArea(availableArea));

    // If the scale changed, free the texture. The next time
    // `possibleGeneratePreview` is called, the texture will be
    // regenerated.
    if (ImGui::InputFloat("Scale", &scale, 0.1f, 0.1f, "%.2f")) {
      scale = std::clamp(scale, 0.1f, 1.0f);

      // Should be its own wrapper but  ¯\_(ツ)_/¯
      UnloadTexture(*texture);
      texture.reset();
    }
  }
  ImGui::EndGroupPanel();
};

auto rt::PreviewTexture::GetImTextureID() const -> ImTextureID {
  assert(texture.has_value() && "Texture should be generated by now...");
  return reinterpret_cast<ImTextureID>(&texture->id);
}

auto rt::PreviewTexture::FitDisplayArea(ImVec2 availableArea) const -> ImVec2 {
  assert(texture.has_value() && "Texture should be generated by now...");
  return EditorUtils::FitIntoArea(availableArea, ImVec2(texture->width, texture->height));
}

