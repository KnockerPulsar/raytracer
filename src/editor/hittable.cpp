#include "hittable.h"

#include <imgui.h>

auto rt::editor::Hittable::getTransformation() const -> const Transformation & { return transformation; }
auto rt::editor::Hittable::getTransformation() -> Transformation & { return transformation; }

void rt::editor::Hittable::setName(std::string newName) { name = newName; }
auto rt::editor::Hittable::getName() const -> std::string { return name; }

auto rt::editor::Hittable::setMaterial(std::shared_ptr<Material> newMat) -> void { material = newMat; }
auto rt::editor::Hittable::getMaterial() const -> Material& { return *material; }

bool rt::editor::Hittable::OnImgui() {
  auto changed = transformation.OnImgui();
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
  return changed;
}
