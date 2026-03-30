#pragma once

#include "data_structures/vec3.h"
#include "materials/material.h"

#include "transformation.h"

#include <memory>

namespace rt::editor {
  class Hittable {
  public:
    void setTranslation(vec3 translation);
    auto getTranslation() const -> vec3;

    void setRotationEuler(vec3 eulerRotation);
    void rotateDelta(vec3 rotationDelta);
    auto getRotationEuler() const -> vec3;

    void setName(std::string newName);
    auto getName() const -> std::string;

    auto setMaterial(std::shared_ptr<Material> newMat) -> void;

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
    template <typename T> friend class HittableBuilder;
    Transformation transformation;
    std::shared_ptr<Material> material;
    std::string name;
  };
}
