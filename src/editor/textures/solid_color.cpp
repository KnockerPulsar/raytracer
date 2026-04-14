#include "solid_color.h"

#include "editor/Utils.h"
#include "textures/SolidColor.h"

rt::editor::SolidColor::SolidColor(vec3 color) : color(color) {}

void rt::editor::SolidColor::OnImgui() {
  Texture::OnImgui();

  std::string id = EditorUtils::GetIDFromPointer(this);
  ImGui::ColorEdit3(("Color##" + id).c_str(), &color.x);
}

auto rt::editor::SolidColor::createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  return std::make_shared<raytracer::SolidColor>(color);
}
