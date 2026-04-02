#include "metal.h"

#include "editor/textures/solid_color.h"
#include "materials/Metal.h"

rt::editor::Metal::Metal(vec3 albedo, float fuzz)
    : albedo(std::make_shared<SolidColor>(albedo)), fuzz(fuzz < 1 ? fuzz : 1) {}

void rt::editor::Metal::OnImgui() {
  albedo->OnImgui();
  ImGui::DragFloat("Fuzziness", &fuzz, 0.05, 0, 1);
}

auto rt::editor::Metal::toRaytracerMaterial() const -> std::shared_ptr<raytracer::Material> {
  return std::make_shared<raytracer::Metal>(albedo->toRaytracerTexture());
}
