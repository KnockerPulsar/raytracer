#pragma once
#include "../data_structures/vec3.h"
#include "SolidColor.h"
#include "Texture.h"
#include "imgui.h"
#include <future>
#include <memory>
#include <raylib.h>

namespace rt {
  class CheckerTexture : public Texture {
  private:
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
    float                    scale;

  public:
    CheckerTexture() = default;
    CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd, float s = 6.0f)
        : even(_even), odd(_odd), scale(s) {}

    CheckerTexture(vec3 c1, vec3 c2, float s = 6.0f)
        : even(std::make_shared<SolidColor>(c1)), odd(std::make_shared<SolidColor>(c2)), scale(s) {}

    void FromColors(vec3 c1, vec3 c2, float s) { *this = CheckerTexture(c1, c2, s); }

    virtual json toJson() const override {
      return json{{"type", "checker"}, {"even", even->toJson()}, {"odd", odd->toJson()}, {"scale", scale}};
    }

    virtual void GetTexture(const json &j) override {
      FromColors(j["even"].get<vec3>(), j["odd"].get<vec3>(), j["scale"].get<float>());
    }

    virtual vec3 Value(float u, float v, const vec3 &p) const override {
      float sines = sin(scale * p.x) * sin(scale * p.y) * sin(scale * p.z);
      if (sines < 0)
        return odd->Value(u, v, p) * multiplier;

      return even->Value(u, v, p) * multiplier;
    }

    void generatePreview() override {
      Texture::generatePreviewUtil([&](float u, float v) { return vec3(u, v , 1); });
    }

    virtual void OnDerivedImgui() override {
      even->OnBaseImgui();
      odd->OnBaseImgui();
      ImGui::DragFloat("Checker scale", &scale);

      ImGui::Spacing();
    }
  };

  inline void from_json(const json &j, CheckerTexture &ct) { ct.GetTexture(j); }
  inline void to_json(json &j, const CheckerTexture &ct) { j = ct.toJson(); }

} // namespace rt