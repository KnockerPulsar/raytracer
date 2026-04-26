#pragma once
#include "common/Defs.h"
#include "app.h"

// TODO to be nuked
namespace rt {
  namespace raytracer { class Raytracer; }
  class IState {
  public:
    IState(App *app) : app(app) {}

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onUpdate() = 0;

  protected:
    App *app;
  };
} // namespace rt
