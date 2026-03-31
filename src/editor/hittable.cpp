#include "hittable.h"

#include <imgui.h>

void rt::editor::Hittable::setTranslation(vec3 translation) { transformation.setTranslation(translation); }
vec3 rt::editor::Hittable::getTranslation() const { return transformation.getTranslation(); }

void rt::editor::Hittable::setRotationEuler(vec3 eulerRotation) { transformation.setRotationEuler(eulerRotation); }
void rt::editor::Hittable::rotateDelta(vec3 rotationDelta) { transformation.rotateDelta(rotationDelta); }
vec3 rt::editor::Hittable::getRotationEuler() const { return transformation.getRotationEuler(); }

void rt::editor::Hittable::setName(std::string newName) { name = newName; }
auto rt::editor::Hittable::getName() const -> std::string { return name; }

auto rt::editor::Hittable::setMaterial(std::shared_ptr<Material> newMat) -> void { material = newMat; }

void rt::editor::Hittable::OnImgui() {
  transformation.OnImgui();
  ImGui::Spacing();
  // TODO should this really be in the base hittable and not the editor?
  // if (material) { // TODO should I enforce all hittables to have a material?
  //   if (auto const newMat = rt::editor::Editor::MaterialChanger()) {
  //     material = newMat.value();
  //   }
  //
  //   ImGui::Spacing();
  //
  //   material->OnImgui();
  // }
}
