#pragma once
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

  class AsyncRenderData;
  class Pixel;
  class Scene;

  enum SceneID { scene1, scene2, random, random_moving, two_spheres, earth, light, cornell };

  class RenderAsync {
  public:
    static pair<int, int> GetThreadJobSlice(int totalJobs, int threadIndex);

    // Need to pass Workers by reference since we can't return a copy of
    // something that contains a future.
    static AsyncRenderData Perpare(int imageWidth, int imageHeight);

    static void BlitToBuffer(vector<Pixel> &pixelJobs, int drawStart, int drawEnd, RenderTexture2D &screenBuffer);

    static bool RenderFinished(AsyncRenderData &ard, bool& allFinished);

    // Causes all threads to exit and join. Clears `ard.threads`
    static void Shutdown(AsyncRenderData &ard);

    static void RenderImGui(bool showProg, AsyncRenderData &ard);

    static void ResetThreads(AsyncRenderData &ard);
  };

} // namespace rt