#pragma once
#include "../data_structures/vec3.h"
#include "SolidColor.h"
#include "Texture.h"

#include <imgui.h>
#include <raylib.h>

#include <memory>

namespace rt {
  class CheckerTexture : public Texture {
  private:
    std::shared_ptr<Texture> const even;
    std::shared_ptr<Texture> const odd;
    float                    scale;

  public:
    CheckerTexture() = default;
    CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd, float s = 6.0f)
        : even(_even), odd(_odd), scale(s) {}

    CheckerTexture(vec3 c1, vec3 c2, float s = 6.0f)
        : even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)), scale(s) {}

    CheckerTexture(const json &json)
        : CheckerTexture(json["even"].get<vec3>(), json["odd"].get<vec3>(), json["scale"].get<float>()) {}

    virtual json toJson() const override {
      return json{{"type", "checker"}, {"even", even->toJson()}, {"odd", odd->toJson()}, {"scale", scale}};
    }

    virtual vec3 Value(float u, float v, const vec3 &p) const override {
      float sines = sin(scale * p.x) * sin(scale * p.y) * sin(scale * p.z);
      if (sines < 0)
        return odd->Value(u, v, p) * multiplier;

      return even->Value(u, v, p) * multiplier;
    }

    virtual void OnImgui() override {
      even->OnImgui();
      odd->OnImgui();
      ImGui::DragFloat("Checker scale", &scale);

      ImGui::Spacing();

      Texture::OnImgui();
    }
  };

  inline void to_json(json &j, const CheckerTexture &ct) { j = ct.toJson(); }

} // namespace rt
