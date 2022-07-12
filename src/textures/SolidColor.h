#pragma once
#include "../data_structures/vec3.h"
#include "../../vendor/rlImGui/imgui/imgui.h"
#include "Texture.h"
#include <raylib.h>

namespace rt {
  class SolidColor : public Texture {
  public:
    vec3 color;

    SolidColor() = default;
    SolidColor(vec3 c) : color(c) {}

    SolidColor(float r, float g, float b) : color(r, g, b) {}

    virtual json toJson() const override { return json{{"type", "solid_color"}, {"color", color}}; }
    virtual void GetTexture(const json &j) override { color = j["color"].get<vec3>(); }

    virtual vec3 Value(float u, float v, const vec3 &p) const override { return color * intensity; }

    virtual void OnImgui() override {
      ImGui::ColorEdit3("Albedo color", &color.x);
    }
  };

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SolidColor, color);
} // namespace rt