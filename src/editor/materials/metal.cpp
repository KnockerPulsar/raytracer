#include "metal.h"

#include "editor/textures/solid_color.h"
#include "materials/Metal.h"

rt::editor::Metal::Metal(vec3 albedo, float fuzz)
    : albedo(std::make_shared<SolidColor>(albedo)), fuzz(fuzz < 1 ? fuzz : 1) {}

bool rt::editor::Metal::OnImgui() {
  auto const albedoChanged = albedo->OnImgui();
  auto const fuzzinessChanged = ImGui::DragFloat("Fuzziness", &fuzz, 0.05, 0, 1);

  return albedoChanged || fuzzinessChanged;
}

auto rt::editor::Metal::toRaytracerMaterial() const -> std::unique_ptr<raytracer::Material> {
  return std::make_unique<raytracer::Metal>(albedo->createRaytracerTexture(), fuzz);
}
