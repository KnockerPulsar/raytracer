#pragma once

#include "editor/IImguiDrawable.h"
#include <memory>

namespace rt {
  class Ray;
  class HitRecord;

  namespace raytracer { class Material; }

  namespace editor {
    class Material : public IImguiDrawable {
    public:
      virtual auto toRaytracerMaterial() const -> std::unique_ptr<raytracer::Material> = 0;
      // TODO ImGui and serailization
    };
  } // namespace editor
} // namespace rt
