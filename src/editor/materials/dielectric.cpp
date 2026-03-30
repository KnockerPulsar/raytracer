#include "dielectric.h"

#include "editor/textures/solid_color.h"

rt::editor::Dielectric::Dielectric(float refractionIndex, vec3 tint)
    : albedo(std::make_shared<SolidColor>(tint)), refractionIndex(refractionIndex) {}

void rt::editor::Dielectric::OnImgui() {
  albedo->OnImgui();
  ImGui::DragFloat("Refraction Index", &refractionIndex, 0.01, 1, 100);
}
