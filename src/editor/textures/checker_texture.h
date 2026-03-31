#pragma once

#include "texture.h"
#include "solid_color.h"

namespace rt::editor {
  class CheckerTexture : public Texture {
    public:
      CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd, float s = 6.0f);
      CheckerTexture(vec3 c1, vec3 c2, float s = 6.0f);

      virtual void OnImgui() override;

    private:
      std::shared_ptr<Texture> const even;
      std::shared_ptr<Texture> const odd;
      float                          scale;
  };
}
