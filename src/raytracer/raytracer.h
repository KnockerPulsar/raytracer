#include "common/IState.h"
#include "common/Ray.h"
#include "common/AsyncRenderData.h"

#include <raylib.h>
#include <rlImGui.h>
#include <stb_image_write.h>

namespace rt::raytracer {
  class Scene;
  class Config;
  class Raytracer : public IState {
  public:
    Raytracer(App *const app, Config &&config, bool saveOnRender, glm::ivec2 editorSize);

    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void onUpdate() override;

  private:
    void startRaytracing();
    void BlitToBuffer();
    bool onFinished();
    void RenderImGui();
    void Autosave();

    static void TraceRay(AsyncRenderData &ard, int threadIndex);
    static vec3 RayColor(const rt::Ray &r, std::unique_ptr<Scene> const &compiledScene, int depth);

    bool allFinished = false;

    AsyncRenderData ard;
    bool const saveOnRender;
    glm::ivec2 const editorSize;

    struct ViewState {
      bool showProgress = true;
      bool detailedThreadProgress = false;
    } viewState;
  };
} // namespace rt
