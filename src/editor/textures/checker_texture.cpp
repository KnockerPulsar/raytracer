#include "checker_texture.h"
#include "textures/CheckerTexture.h"

#include <imgui.h>

rt::editor::CheckerTexture::CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd, float s)
    : even(_even), odd(_odd), scale(s) {}

rt::editor::CheckerTexture::CheckerTexture(vec3 c1, vec3 c2, float s)
    : even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)), scale(s) {}

void rt::editor::CheckerTexture::OnImgui() {
  Texture::OnImgui();

  if (ImGui::DragFloat("Checker scale", &scale))
    invalidatePreview();

  ImGui::Spacing();
}

auto rt::editor::CheckerTexture::createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  return std::make_shared<raytracer::CheckerTexture>(even->createRaytracerTexture(), odd->createRaytracerTexture(), scale);
}
