#include "solid_color.h"

#include "editor/Utils.h"
#include "textures/SolidColor.h"

rt::editor::SolidColor::SolidColor(vec3 color) : color(color) {}

void rt::editor::SolidColor::OnImgui() {
  std::string id = EditorUtils::GetIDFromPointer(this);
  ImGui::ColorEdit3(("Color##" + id).c_str(), &color.x);

  Texture::OnImgui();
}

auto rt::editor::SolidColor::toRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  return std::make_shared<raytracer::SolidColor>(color);
}
