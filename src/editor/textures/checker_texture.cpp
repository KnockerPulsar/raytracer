#include "checker_texture.h"

#include "textures/CheckerTexture.h"

rt::editor::CheckerTexture::CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd, float s)
    : even(_even), odd(_odd), scale(s) {}

rt::editor::CheckerTexture::CheckerTexture(vec3 c1, vec3 c2, float s)
    : even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)), scale(s) {}

void rt::editor::CheckerTexture::OnImgui() {
  even->OnImgui();
  odd->OnImgui();

  ImGui::DragFloat("Checker scale", &scale);

  ImGui::Spacing();
}

auto rt::editor::CheckerTexture::toRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  return std::make_shared<raytracer::CheckerTexture>(even->toRaytracerTexture(), odd->toRaytracerTexture(), scale);
}
