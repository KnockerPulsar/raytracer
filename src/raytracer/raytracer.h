#include "common/IState.h"
#include "data_structures/JobQueue.h"
#include "common/Ray.h"

#include <raylib.h>
#include <rlImGui.h>
#include <stb_image_write.h>

namespace rt::raytracer {
  class Scene;
  class Raytracer : public IState {
  public:
    Raytracer(App *const app, AsyncRenderData &ard, std::shared_ptr<Scene> const &compiledScene);

    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void onUpdate() override;

  private:
    void startRaytracing();
    void BlitToBuffer();
    bool onFinished();
    void RenderImGui();
    void Autosave();

    static void TraceRay(AsyncRenderData &ard, std::shared_ptr<Scene> const &compiledScene, int threadIndex);
    static vec3 RayColor(const rt::Ray &r, std::shared_ptr<Scene> const &compiledScene, int depth);

    bool allFinished = false;
    AsyncRenderData &ard;
    std::shared_ptr<Scene> const compiledScene;

    struct ViewState {
      bool showProgress = true;
      bool detailedThreadProgress = false;
    } viewState;
  };
} // namespace rt
