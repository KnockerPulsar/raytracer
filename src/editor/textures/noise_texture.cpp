#include "noise_texture.h"
#include "textures/NoiseTexture.h"

rt::editor::NoiseTexture::NoiseTexture(float scale, float turbulanceScale, vec3 baseColor)
    : scale(scale), turbScale(turbulanceScale), baseColor(baseColor) {}

bool rt::editor::NoiseTexture::OnImgui() {
  return previewHeader("Noise Texture", [&] {
    auto const noiseScaleChanged      = ImGui::DragFloat("Noise scale", &scale, 0.1f);
    auto const turbulanceScaleChanged = ImGui::DragFloat("Turbulance scale", &turbScale, 0.1f);
    auto const baseColorChanged       = ImGui::ColorEdit3("Base color", &baseColor.x);
    return noiseScaleChanged || turbulanceScaleChanged || baseColorChanged;
  });
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
