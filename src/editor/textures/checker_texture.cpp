#include "checker_texture.h"
#include "textures/CheckerTexture.h"

#include <imgui.h>

rt::editor::CheckerTexture::CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd, float s)
    : even(_even), odd(_odd), scale(s) {}

rt::editor::CheckerTexture::CheckerTexture(vec3 c1, vec3 c2, float s)
    : even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)), scale(s) {}

bool rt::editor::CheckerTexture::OnImgui() {
  auto const previewChanged =
      previewHeader("Checker Texture", [&] { return ImGui::DragFloat("Checker scale", &scale); });

  ImGui::Indent(10);
  ImGui::SetNextItemOpen(false, ImGuiCond_Once);
  auto const evenTextureChanged = even->OnImgui();

  ImGui::SetNextItemOpen(false, ImGuiCond_Once);
  auto const oddTextureChanged = odd->OnImgui();
  ImGui::Indent(-10);

  if (evenTextureChanged || oddTextureChanged)
    invalidatePreview();

  return evenTextureChanged || oddTextureChanged;
}

auto rt::editor::CheckerTexture::createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  return std::make_shared<raytracer::CheckerTexture>(even->createRaytracerTexture(), odd->createRaytracerTexture(), scale);
}
