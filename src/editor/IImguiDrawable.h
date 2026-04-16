#pragma once

namespace rt {
  class IImguiDrawable {
  public:
    virtual bool OnImgui() { return false; };
  };
} // namespace rt
