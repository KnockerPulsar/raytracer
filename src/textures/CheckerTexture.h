#pragma once
#include "../data_structures/vec3.h"
#include "SolidColor.h"
#include "Texture.h"
#include <future>
#include <memory>
#include <raylib.h>

namespace rt {
  class CheckerTexture : public Texture {
  private:
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
    float                    scale = 1.0;

  public:
    CheckerTexture() = default;
    CheckerTexture(std::shared_ptr<Texture> _even,
                   std::shared_ptr<Texture> _odd, float s = 1.0f)
        : even(_even), odd(_odd), scale(s) {}

    CheckerTexture(vec3 c1, vec3 c2, float s = 1.0f)
        : even(std::make_shared<SolidColor>(c1)),
          odd(std::make_shared<SolidColor>(c2)), scale(s) {}

    void FromColors(vec3 c1, vec3 c2, float s) {
      *this = CheckerTexture(c1, c2, s);
    }

    virtual json GetJson() const override {
      return json{{"type", "checker"},
                  {"even", even->GetJson()},
                  {"odd", odd->GetJson()},
                  {"scale", scale}};
    }

    virtual void GetTexture(const json &j) override {
      FromColors(
          j["even"].get<vec3>(), j["odd"].get<vec3>(), j["scale"].get<float>());
    }

    virtual vec3 Value(float u, float v, const vec3 &p) const override {
      float sines = sin(scale * p.x) * sin(scale * p.y) * sin(scale * p.z);
      if (sines < 0)
        return odd->Value(u, v, p);
      else
        return even->Value(u, v, p);
    }
  };

  inline void from_json(const json &j, CheckerTexture &ct) { ct.GetTexture(j); }
  inline void to_json(json &j, const CheckerTexture &ct) { j = ct.GetJson(); }

} // namespace rt