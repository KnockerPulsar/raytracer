#pragma once
#include "Defs.h"
#include "app.h"

namespace rt {
  class IState {
  public:
    IState(App *const app) : app(app) {}
    void setNextState(sPtr<IState> const &nextState) { this->nextState = nextState; }

    virtual void         onEnter()  = 0;
    virtual void         onExit()   = 0;
    virtual void         onUpdate() = 0;
    virtual sPtr<IState> toNextState() {
      this->onExit();
      nextState->onEnter();
      return nextState;
    };

    virtual void changeScene(Scene *scene) {}
    Scene       *getScene() const { return app->getScene(); }

  protected:
    sPtr<IState> nextState;
    App *const   app;
  };
} // namespace rt
