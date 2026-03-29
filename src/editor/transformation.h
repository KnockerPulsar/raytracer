#pragma once

#include "editor/IImguiDrawable.h"

namespace rt::editor {
  class Transformation: public IImguiDrawable {
  public:
    virtual void OnImgui() override;
  };
};
