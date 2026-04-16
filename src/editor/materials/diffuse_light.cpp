#include "diffuse_light.h"
#include "materials/DiffuseLight.h"

rt::editor::DiffuseLight::DiffuseLight(vec3 color) : emssiveTex(ms<SolidColor>(color)) {}

bool rt::editor::DiffuseLight::OnImgui() {
  return emssiveTex->OnImgui();
  // ImGui::DragFloat("Emissive intensity", &emissiveIntensity, 0.1, 1.0f);
}

auto rt::editor::DiffuseLight::toRaytracerMaterial() const -> std::unique_ptr<raytracer::Material> {
  return std::make_unique<raytracer::DiffuseLight>(emssiveTex->createRaytracerTexture());
}
