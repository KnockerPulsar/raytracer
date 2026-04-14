#include "noise_texture.h"
#include "textures/NoiseTexture.h"

rt::editor::NoiseTexture::NoiseTexture(float scale, float turbulanceScale, vec3 baseColor)
    : scale(scale), turbScale(turbulanceScale), baseColor(baseColor) {}

void rt::editor::NoiseTexture::OnImgui() {
  if (ImGui::DragFloat("Noise scale", &scale, 0.1f) || ImGui::DragFloat("Turbulance scale", &turbScale, 0.1f) ||
      ImGui::ColorEdit3("Base color", &baseColor.x))
    invalidatePreview();

  ImGui::Spacing();

  Texture::OnImgui();
}

auto rt::editor::NoiseTexture::createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  // TODO Right now, the editor impl doesn't generate perlin noise. When it
  // does eventually do, it should be a shared handle that both the editor and
  // raytracer impls can use.
  //
  // Then, the raytracer impl won't need to store scales. And the preview
  // should match the raytraced result.
  return std::make_shared<raytracer::NoiseTexture>(scale, turbScale, baseColor);
}
