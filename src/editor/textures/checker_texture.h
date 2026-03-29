#pragma once

#include "texture.h"

namespace rt::editor {
  class CheckerTexture : public Texture {
    public:
      CheckerTexture(vec3 even, vec3 odd, float scale = 6.0f);

    virtual void OnImgui() override {
      // even->OnImgui();
      // odd->OnImgui();
      //
      // ImGui::DragFloat("Checker scale", &scale);
      //
      // ImGui::Spacing();
    }
  };
}
