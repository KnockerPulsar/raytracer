#pragma once

#include "materials/material.h"

#include "transformation.h"

#include <memory>

namespace rt {
  class Raytracable;
  namespace editor {
    class Hittable : public IImguiDrawable {
    public:
      // TODO default material? Default name?
      Hittable() = default;

      auto getTransformation() const -> const Transformation &;
      auto getTransformation() -> Transformation &;

      void setName(std::string newName);
      auto getName() const -> std::string;

      auto setMaterial(std::shared_ptr<Material> newMat) -> void;
      auto getMaterial() const -> std::shared_ptr<Material>;

      virtual std::optional<float> hit(const rt::Ray &worldspaceRay) const = 0;

      virtual void OnImgui() override;

      virtual auto toRaytracable() const -> std::shared_ptr<rt::Raytracable> = 0;

    private:
      template <typename T> friend class HittableBuilder;
      Transformation            transformation;
      std::shared_ptr<Material> material;
      std::string               name;
    };
  } // namespace editor
} // namespace rt
