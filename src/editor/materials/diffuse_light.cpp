#include "diffuse_light.h"
#include "materials/DiffuseLight.h"

rt::editor::DiffuseLight::DiffuseLight(vec3 color) : emssiveTex(ms<SolidColor>(color)) {}

void rt::editor::DiffuseLight::OnImgui() {
  emssiveTex->OnImgui();
  // ImGui::DragFloat("Emissive intensity", &emissiveIntensity, 0.1, 1.0f);
}

auto rt::editor::DiffuseLight::toRaytracerMaterial() const -> std::shared_ptr<raytracer::Material> {
  return std::make_shared<raytracer::DiffuseLight>(emssiveTex->toRaytracerTexture());
}
