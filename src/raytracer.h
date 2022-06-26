#include "AsyncRenderData.h"
#include "Constants.h"
#include "IState.h"
#include "RenderAsync.h"
#include <algorithm>
#include <raylib.h>

namespace rt {
  class Raytracer : public IState {
    AsyncRenderData &ard;
    bool allFinished;

  public:
    bool showProg = true;

    Raytracer(AsyncRenderData &ard) : ard(ard) {}

    virtual void onEnter() { RenderAsync::Start(ard); }

    virtual void onExit() {
      RenderAsync::Shutdown(ard) ;
      allFinished = false;
      // Clear raytracing render texture.
    }

    virtual void onUpdate() {
      BeginDrawing();
      // Check on thread progress.
      RenderAsync::RenderFinished(ard, allFinished);
      RenderAsync::RenderImGui(showProg, ard);

      EndDrawing();
    }
  };
} // namespace rt