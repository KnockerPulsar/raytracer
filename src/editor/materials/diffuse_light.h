#pragma once

#include "material.h"
#include "editor/textures/solid_color.h"

#include "data_structures/vec3.h"

namespace rt::editor {
  class DiffuseLight : public Material {
  public:
    // TODO textured lights
    DiffuseLight(vec3 color);
    virtual void OnImgui() override;

    auto toRaytracerMaterial() const -> std::unique_ptr<raytracer::Material> override;
    // json toJson() const override { return json{{"type", "diffuse_light"}, {"texture", emssiveTex->toJson()}}; }
  private:
    std::shared_ptr<SolidColor> emssiveTex;
    // TODO hook up intensity
    // float emissiveIntensity = 1.0f;
  };

  // inline void from_json(const json &j, DiffuseLight &dl) { dl.emssiveTex = TextureFactory::FromJson(j["texture"]); }

  // inline void to_json(json &j, const DiffuseLight &dl) { j = dl.toJson(); }
} // namespace rt::editor
