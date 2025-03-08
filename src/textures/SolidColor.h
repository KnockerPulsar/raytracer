#pragma once
#include "imgui.h"
#include "../data_structures/vec3.h"
#include "Texture.h"
#include "editor/Utils.h"
#include "json.hpp"
#include "textures/ImageTexture.h"
#include <raylib.h>
#include <string>

namespace rt {
  class SolidColor : public Texture {
  public:
    vec3 color;

    SolidColor() = default;
    SolidColor(vec3 c) : color(c) {}

    SolidColor(float r, float g, float b) : color(r, g, b) {}

    virtual json toJson() const override { return json{{"type", "solid_color"}, {"color", color}}; }
    virtual void GetTexture(const json &j) override { color = j["color"].get<vec3>(); }

    virtual vec3 Value(float u, float v, const vec3 &p) const override { return color * multiplier; }

    virtual void OnImgui() override {
      std::string id = EditorUtils::GetIDFromPointer(this);
      ImGui::ColorEdit3(("Color##" + id).c_str(), &color.x);

      Texture::previewOrGenerate();
    }

    void generatePreview() override {
      Texture::generatePreviewUtil([](float u, float v) { return vec3(u, v, 0); });
    }
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SolidColor, color);
} // namespace rt
