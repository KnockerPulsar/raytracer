#pragma once
#include "Defs.h"
#include "app.h"

namespace rt {
  class IState {
  public:
    sPtr<IState> nextState;
    App         *app;

    virtual void         onEnter()  = 0;
    virtual void         onExit()   = 0;
    virtual void         onUpdate() = 0;
    virtual sPtr<IState> toNextState() {
      this->onExit();
      nextState->onEnter();
      return nextState;
    };

    virtual void changeScene(Scene *scene) {}
    Scene       *getScene() { return app->getScene(); }
    Camera      *getCamera() { return &getScene()->cam; }
  };
} // namespace rt