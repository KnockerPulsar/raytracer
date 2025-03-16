#include "AsyncRenderData.h"
#include "IState.h"
#include "RenderAsync.h"
#include "data_structures/JobQueue.h"

#include <raylib.h>
#include <rlImGui.h>
#include <stb_image_write.h>

#include <functional>

using std::ref;

namespace rt {
  class Raytracer : public IState {
  public:
    Raytracer(AsyncRenderData &ard);

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
