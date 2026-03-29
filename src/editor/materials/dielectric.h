#pragma once

#include "material.h"

namespace rt::editor {
  class Dielectric : public Material {
  public:
    Dielectric(float refractionIndex);
    virtual void OnImgui() override {
      // albedo->OnImgui();
      // ImGui::DragFloat("Refraction Index", &refractionIndex, 0.01, 1, 100);
    }
  };

  // inline void from_json(const json &j, Dielectric &d) {
  //   d.refractionIndex = j["refraction_index"].get<float>();
  //   d.albedo          = TextureFactory::FromJson(j["texture"]);
  // }
  //
  // inline void to_json(json &j, const Dielectric &d) { j = d.toJson(); }
} // namespace rt
