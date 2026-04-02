#pragma once
#include "common/Defs.h"
#include "app.h"

namespace rt {
  namespace raytracer { class Raytracer; }
  class IState {
  public:
    IState(App *app) : app(app) {}

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onUpdate() = 0;

    void rebuildRaytracer(std::shared_ptr<raytracer::Raytracer> const& newRaytracer) {
      app->rebuildRaytracer(newRaytracer);
    }

  protected:
    App *app;
  };
} // namespace rt
