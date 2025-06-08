#pragma once

#include "GroupPanel.h"
#include "IImguiDrawable.h"
#include <imgui.h>
#include <optional>
#include <raylib.h>

namespace rt {
  class Texture;
  class PreviewTexture: public IImguiDrawable {
  public:
    ~PreviewTexture();

    void possiblyGeneratePreview(rt::Texture &tex);
    void OnImgui() override;

    auto const inline static minDimensionSize = 32;

  private:
    auto GetImTextureID() const -> ImTextureID;
    auto FitDisplayArea(ImVec2 availableArea) const -> ImVec2;

    std::optional<Texture2D> texture;
    float scale{0.1};
  };
} // namespace rt
