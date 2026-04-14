#pragma once

#include "editor/IImguiDrawable.h"
#include "textures/Texture.h"

#include <imgui.h>
#include <memory>
#include <optional>
#include <raylib.h>

namespace rt::editor {
  class Texture;
  class PreviewTexture: public IImguiDrawable {
  public:
    PreviewTexture();
    ~PreviewTexture();

    void possiblyGeneratePreview(Texture &tex);
    void OnImgui() override;

    auto getRaytracerTexture() const -> std::shared_ptr<raytracer::Texture>;

    void invalidate();

  private:
    auto GetImTextureID() const -> ImTextureID;
    auto FitDisplayArea(ImVec2 availableArea) const -> ImVec2;

    auto const inline static minDimensionSize = 32;

    std::function<vec3(float, float)> const projection;
    std::optional<Texture2D>            texture;
    std::shared_ptr<raytracer::Texture> raytracerTexture;
    float                               scale{0.1};
  };
} // namespace rt
