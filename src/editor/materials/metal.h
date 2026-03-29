#pragma once

#include "material.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class Metal : public Material {
  public:
    Metal(vec3 albedo, float fuzz);
    virtual void OnImgui() override {
      // albedo->OnImgui();
      // ImGui::DragFloat("Fuzziness", &fuzz, 0.05, 0, 1);
    }

    // json toJson() const override {
    //   return json{
    //       {"type", "metal"},
    //       {"fuzz", fuzz},
    //       {"texture", albedo->toJson()},
    //   };
    // }
  };

  // inline void from_json(const json &j, Metal &m) {
  //   m.fuzz   = j["fuzz"];
  //   m.albedo = TextureFactory::FromJson(j["texture"]);
  // }
  //
  // inline void to_json(json &j, const Metal &m) { j = m.toJson(); }
}
