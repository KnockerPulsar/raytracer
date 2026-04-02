#pragma once

#include "material.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class Texture;

  class Lambertian : public Material {
  public:
    Lambertian(vec3 albedo);
    Lambertian(std::shared_ptr<Texture> albedo);

    virtual void OnImgui() override;

    auto toRaytracerMaterial() const -> std::shared_ptr<raytracer::Material> override;
    // json toJson() const override { return json{{"type", "lambertian"}, {"texture", albedo->toJson()}}; }

  private:
    std::shared_ptr<Texture> albedo;
  };
  // inline void from_json(const json &j, Lambertian &l) { l.albedo = TextureFactory::FromJson(j["texture"]); }
  //
  // inline void to_json(json &j, const Lambertian &l) { j = l.toJson(); }
}
