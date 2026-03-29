#include "AsyncRenderData.h"
#include "IState.h"
#include "data_structures/JobQueue.h"

#include <raylib.h>
#include <rlImGui.h>
#include <stb_image_write.h>

namespace rt {
  class Raytracer : public IState {
  public:
    Raytracer(App *app, AsyncRenderData &ard);

    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void onUpdate() override;

  private:
    void startRaytracing();
    void BlitToBuffer();
    bool onFinished();
    void RenderImGui();
    void Autosave();

    bool allFinished = false;
    AsyncRenderData &ard;

    struct ViewState {
      bool showProgress = true;
      bool detailedThreadProgress = false;
    } viewState;
  };
} // namespace rt
