#pragma once

#include "texture.h"

namespace rt::editor {
  class NoiseTexture : public Texture {
  public:
    NoiseTexture(float scale);
    virtual void OnImgui() override {

      // ImGui::DragFloat("Noise scale", &scale, 0.1f);
      // ImGui::DragFloat("Turbulance scale", &turbScale, 0.1f);
      // ImGui::ColorEdit3("Base color", &baseColor.x);

      ImGui::Spacing();

      Texture::OnImgui();
    }
  };
} // namespace rt::editor
