#pragma once

#include "texture.h"

namespace rt::editor {
  class SolidColor : public Texture {
  public:
    SolidColor(vec3 color);
    virtual void OnImgui() override;

  private:
    vec3 color;
  };
} // namespace rt::editor
