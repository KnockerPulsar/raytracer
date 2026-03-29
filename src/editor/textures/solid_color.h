#pragma once

#include "editor/Utils.h"
#include "texture.h"

namespace rt::editor {
  class SolidColor : public Texture {
    virtual void OnImgui() override {
      std::string id = EditorUtils::GetIDFromPointer(this);
      ImGui::ColorEdit3(("Color##" + id).c_str(), &color.x);

      Texture::OnImgui();
    }

  };
}

