#include "dielectric.h"

#include "editor/textures/solid_color.h"
#include "materials/Dielectric.h"

rt::editor::Dielectric::Dielectric(float refractionIndex, vec3 tint)
    : albedo(std::make_shared<SolidColor>(tint)), refractionIndex(refractionIndex) {}

void rt::editor::Dielectric::OnImgui() {
  albedo->OnImgui();
  ImGui::DragFloat("Refraction Index", &refractionIndex, 0.01, 1, 100);
}

auto rt::editor::Dielectric::toRaytracerMaterial() const -> std::unique_ptr<raytracer::Material> {
  return std::make_unique<raytracer::Dielectric>(refractionIndex, albedo->toRaytracerTexture());
}
