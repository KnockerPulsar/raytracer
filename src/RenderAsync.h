#pragma once
#include "AsyncRenderData.h"
#include "raylib.h"
#include <future>
#include <utility>
#include <vector>

using std::pair, std::vector, std::future;

// bool -> done or not
// future -> data returned from async
using RunningJob = std::pair<bool, future<void>>;
using Workers    = vector<RunningJob>;

namespace rt {
  class Pixel;
  class Scene;

  enum SceneID {
    scene1,
    scene2,
    random,
    random_moving,
    two_spheres,
    earth,
    light,
    cornell
  };

  class RenderAsync {
  public:
    static pair<int, int> GetThreadJobSlice(int totalJobs, int threadIndex);

    // Need to pass Workers by reference since we can't return a copy of
    // something that contains a future.
    static AsyncRenderData Perpare(int imageWidth, float aspectRatio,
                                   int maxDepth, int samplesPerPixel,
                                   SceneID sceneID, int incRender,
                                   int gridWidth = 11, int gridHeight = 11);

    static void Start(AsyncRenderData &ard);

    static void BlitToBuffer(vector<Pixel> &pixelJobs, int drawStart,
                             int drawEnd, RenderTexture2D &screenBuffer);

    static bool RenderFinished(AsyncRenderData &ard);

    static void Shutdown(bool fullscreen, AsyncRenderData &ard);

    static void RenderImGui(bool showProg, AsyncRenderData &ard);

    static void ResetThreads(AsyncRenderData &ard);

    static void CheckInput(bool &fullscreen, bool &showProg);
  };

} // namespace rt