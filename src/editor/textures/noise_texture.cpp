#include "noise_texture.h"

rt::editor::NoiseTexture::NoiseTexture(float scale, float turbulanceScale, vec3 baseColor)
    : scale(scale), turbScale(turbulanceScale), baseColor(baseColor) {}

void rt::editor::NoiseTexture::OnImgui() {
  ImGui::DragFloat("Noise scale", &scale, 0.1f);
  ImGui::DragFloat("Turbulance scale", &turbScale, 0.1f);
  ImGui::ColorEdit3("Base color", &baseColor.x);

  ImGui::Spacing();

  Texture::OnImgui();
}
