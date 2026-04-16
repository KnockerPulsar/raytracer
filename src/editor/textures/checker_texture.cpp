#include "checker_texture.h"
#include "textures/CheckerTexture.h"

#include <imgui.h>

rt::editor::CheckerTexture::CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd, float s)
    : even(_even), odd(_odd), scale(s) {}

rt::editor::CheckerTexture::CheckerTexture(vec3 c1, vec3 c2, float s)
    : even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)), scale(s) {}

bool rt::editor::CheckerTexture::OnImgui() {
  auto const baseTextureChanged = Texture::OnImgui();
  auto const textureScaleChanged = ImGui::DragFloat("Checker scale", &scale);

  if (textureScaleChanged)
    invalidatePreview();

  ImGui::Spacing();

  auto evenTextureChanged = false;
  if (ImGui::TreeNodeEx("Even Texture", ImGuiTreeNodeFlags_CollapsingHeader)) {
    evenTextureChanged = even->OnImgui();
    ImGui::Spacing();
  }

  auto oddTextureChanged = false;
  if (ImGui::TreeNodeEx("Odd Texture", ImGuiTreeNodeFlags_CollapsingHeader)) {
    oddTextureChanged = odd->OnImgui();
    ImGui::Spacing();
  }

  return baseTextureChanged || textureScaleChanged || evenTextureChanged || oddTextureChanged;
}

auto rt::editor::CheckerTexture::createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  return std::make_shared<raytracer::CheckerTexture>(even->createRaytracerTexture(), odd->createRaytracerTexture(), scale);
}
