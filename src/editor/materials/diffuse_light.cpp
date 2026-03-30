#include "diffuse_light.h"

rt::editor::DiffuseLight::DiffuseLight(vec3 color) {
  auto baseCol = ms<SolidColor>(color);
  emssiveTex   = baseCol;
}

void rt::editor::DiffuseLight::OnImgui() {
  emssiveTex->OnImgui();
  ImGui::DragFloat("Emissive intensity", &emissiveIntensity, 0.1, 1.0f);
}
