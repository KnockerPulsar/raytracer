#include "AsyncRenderData.h"
#include "Constants.h"
#include "IState.h"
#include "RenderAsync.h"
#include <algorithm>
#include <functional>
#include <raylib.h>
#include "Ray.h"


using std::ref;

namespace rt {
  class Raytracer : public IState {
    AsyncRenderData &ard;
    bool             allFinished;

  public:
    bool showProg = true;

    Raytracer(AsyncRenderData &ard) : ard(ard) {}

    virtual void onEnter() { startRaytracing(); }

    virtual void onExit() {
      RenderAsync::Shutdown(ard);
      allFinished = false;
    }

    virtual void onUpdate() {
      BeginDrawing();
      // Check on thread progress.
      RenderAsync::RenderFinished(ard, allFinished);
      RenderAsync::RenderImGui(showProg, ard);

      EndDrawing();
    }

    void startRaytracing() {
      ard.exit = false;

      for (int t = 0; t < NUM_THREADS; t++) {
        ard.threads.push_back(std::make_shared<std::thread>(Ray::Trace, ref(ard), t));
      }
    }
  };
} // namespace rt