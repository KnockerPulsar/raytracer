#pragma once

#include "materials/material.h"

#include "transformation.h"

#include <memory>

namespace rt::editor {
  class Hittable: public IImguiDrawable {
  public:
    auto getTransformation() const -> const Transformation &;
    auto getTransformation() -> Transformation &;

    void setName(std::string newName);
    auto getName() const -> std::string;

    auto setMaterial(std::shared_ptr<Material> newMat) -> void;

    virtual std::optional<float> hit(const rt::Ray &worldspaceRay) const = 0;

    virtual void OnImgui() override;

  private:
    template <typename T> friend class HittableBuilder;
    Transformation transformation;
    std::shared_ptr<Material> material;
    std::string name;
  };
}
