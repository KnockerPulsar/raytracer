#include "solid_color.h"

#include "editor/Utils.h"
#include "textures/SolidColor.h"

rt::editor::SolidColor::SolidColor(vec3 color) : color(color) {}

bool rt::editor::SolidColor::OnImgui() {
  return previewHeader("Solid Color Texture", [&] {
    std::string id = EditorUtils::GetIDFromPointer(this);
    return ImGui::ColorEdit3(("Color##" + id).c_str(), &color.x);
  });
}

auto rt::editor::SolidColor::createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> {
  return std::make_shared<raytracer::SolidColor>(color);
}
