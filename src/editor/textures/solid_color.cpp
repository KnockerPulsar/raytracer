#include "solid_color.h"

#include "editor/Utils.h"
#include "textures/SolidColor.h"

rt::editor::SolidColor::SolidColor(vec3 color) : color(color) {}

bool rt::editor::SolidColor::OnImgui() {
  auto const textureChanged = Texture::OnImgui();

  std::string id = EditorUtils::GetIDFromPointer(this);
  auto const colorChanged = ImGui::ColorEdit3(("Color##" + id).c_str(), &color.x);
  if(colorChanged)
    invalidatePreview();

  return textureChanged || colorChanged;
}

auto rt::editor::SolidColor::createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  return std::make_shared<raytracer::SolidColor>(color);
}
