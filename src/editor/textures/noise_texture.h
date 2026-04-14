#pragma once

#include "texture.h"

namespace rt::editor {
  class NoiseTexture : public Texture {
  public:
    NoiseTexture(float scale, float turbulanceScale = 10, vec3 baseColor = vec3(1));
    virtual void OnImgui() override;

    auto createRaytracerTexture() const -> std::shared_ptr<raytracer::Texture> override;

  private:
    float        scale;
    float        turbScale;
    vec3         baseColor;
  };
} // namespace rt::editor
