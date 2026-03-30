#pragma once

#include "material.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class Texture;

  class Metal : public Material {
  public:
    Metal(vec3 albedo, float fuzz);
    virtual void OnImgui() override;

    // json toJson() const override {
    //   return json{
    //       {"type", "metal"},
    //       {"fuzz", fuzz},
    //       {"texture", albedo->toJson()},
    //   };
    // }
  private:
    std::shared_ptr<Texture> albedo;
    float                    fuzz;
  };

  // inline void from_json(const json &j, Metal &m) {
  //   m.fuzz   = j["fuzz"];
  //   m.albedo = TextureFactory::FromJson(j["texture"]);
  // }
  //
  // inline void to_json(json &j, const Metal &m) { j = m.toJson(); }
}
