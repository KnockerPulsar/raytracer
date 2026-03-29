#pragma once

#include <functional>

// Given an ImGui function that indicates whether the value was changed or
// not, calls a user defined callback whenever the value stops changing
//
// Unused at the moment
namespace rt::Editor
{
template <typename T> struct GuiValue {

  GuiValue(T &&init) : value(init) {}

  GuiValue &OnChangeStop(std::function<void()> fn) {
    onChangeStop = fn;
    return *this;
  }

  GuiValue &RenderImGUI(std::function<bool(T &)> imguiRenderer) {
    auto const changing = imguiRenderer(value);

    if (changed && !changing) {
      onChangeStop();
    }

    changed = changing;
    return *this;
  }

  T                     value;
  bool                  changed{false};
  std::function<void()> onChangeStop;
};
}
