#pragma once

#include "editor/IImguiDrawable.h"

#include <cstdint>

namespace rt::editor {
  class Material: public IImguiDrawable {
  public:
    enum class MaterialTypes : uint8_t {
      Diffuse,
      Dielectrical,
      Metallic,
      Emissive,
      MaterialTypesCount,
    };
  };
}
