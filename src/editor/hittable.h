#pragma once

#include "data_structures/vec3.h"
#include "materials/material.h"

#include "transformation.h"

#include <memory>

namespace rt::editor {
  class Hittable {
  public:
    void setTranslation(vec3 translation);
    void rotateDelta(vec3 rotationDelta);
    auto getRotationEuler() const -> vec3;
    auto getTranslation() const -> vec3;
    auto getName() const -> std::string;

    // virtual void OnImgui() override {
    //   transformation.OnImgui();
    //   ImGui::Spacing();
    //   if (material) { // TODO should I enforce all hittables to have a material?
    //     if (auto const newMat = rt::editor::Editor::MaterialChanger()) {
    //       material = newMat.value();
    //     }
    //
    //     ImGui::Spacing();
    //
    //     material->OnImgui();
    //   }
    // }
  private:
    Transformation transformation;
    std::shared_ptr<Material> material;
    std::string name;
  };
}
