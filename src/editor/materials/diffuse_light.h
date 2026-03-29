#pragma once

#include "material.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class DiffuseLight : public Material {
  public:
    DiffuseLight(vec3 color);

    // json toJson() const override { return json{{"type", "diffuse_light"}, {"texture", emssiveTex->toJson()}}; }

    virtual void OnImgui() override {
      // emssiveTex->OnImgui();
      // ImGui::DragFloat("Emissive intensity", &emissiveIntensity, 0.1, 1.0f);
      // emssiveTex->setIntensity(emissiveIntensity);
    }
  };

  // inline void from_json(const json &j, DiffuseLight &dl) { dl.emssiveTex = TextureFactory::FromJson(j["texture"]); }

  // inline void to_json(json &j, const DiffuseLight &dl) { j = dl.toJson(); }
} // namespace rt::editor
