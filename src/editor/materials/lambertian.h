#pragma once

#include "material.h"
#include "editor/textures/texture.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class Lambertian : public Material {
  public:
    Lambertian(vec3 albedo);
    Lambertian(std::shared_ptr<Texture> albedo);

    virtual void OnImgui() override {
      // albedo->OnImgui();
    }

    // json toJson() const override { return json{{"type", "lambertian"}, {"texture", albedo->toJson()}}; }
  };
  // inline void from_json(const json &j, Lambertian &l) { l.albedo = TextureFactory::FromJson(j["texture"]); }
  //
  // inline void to_json(json &j, const Lambertian &l) { j = l.toJson(); }
}
