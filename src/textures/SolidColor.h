#pragma once
#include "../data_structures/vec3.h"
#include "Texture.h"
#include "editor/Utils.h"
#include "textures/ImageTexture.h"

#include <nlohmann-json/json.hpp>
#include <raylib.h>

#include <string>

namespace rt {
  class SolidColor : public Texture {
  public:
    vec3 color;

    SolidColor() = default;
    SolidColor(vec3 c) : color(c) {}

    SolidColor(float r, float g, float b) : color(r, g, b) {}
    SolidColor(const json& j) : color(j["color"].get<vec3>()) {}

    virtual json toJson() const override { return json{{"type", "solid_color"}, {"color", color}}; }

    virtual vec3 Value(float u, float v, const vec3 &p) const override { return color * multiplier; }

    virtual void OnImgui() override {
      std::string id = EditorUtils::GetIDFromPointer(this);
      ImGui::ColorEdit3(("Color##" + id).c_str(), &color.x);

      Texture::previewOrGenerate();
    }
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SolidColor, color);
} // namespace rt
