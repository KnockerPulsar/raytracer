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
    static void ResetThreads(AsyncRenderData &ard);
  };

} // namespace rt