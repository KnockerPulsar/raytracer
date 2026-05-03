#pragma once

#include "common/Ray.h"
#include "common/AsyncRenderData.h"

#include <raylib.h>
#include <rlImGui.h>
#include <stb_image_write.h>

namespace rt::raytracer {
  class Scene;
  class Config;

  struct Frame {
    ::Texture                 texture;
    std::chrono::milliseconds renderTime;
    uint64_t                  seed;
    uint64_t                  numSamples;
    uint64_t                  maxDepth;
  };

  class Raytracer {
  public:
    Raytracer(Config &&config, std::function<void(Frame)>);

    void onUpdate();
    void startRaytracing();
    void stopRaytracing();

    float progress() const;
    bool isRaytracing() const;

  private:
    void RenderImGui();

    static void TraceRay(AsyncRenderData &ard, int threadIndex);
    static vec3 RayColor(const rt::Ray &r, std::unique_ptr<Scene> const &compiledScene, int depth);

    bool raytracingDone = false;

    AsyncRenderData ard;

    struct ViewState {
      bool showProgress = true;
      bool detailedThreadProgress = false;
    } viewState;

    std::function<void(Frame)> const onRenderDone;
  };
} // namespace rt
