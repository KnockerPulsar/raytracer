#pragma once
#include <iostream>
#include "data_structures/Pixel.h"
#include "Scene.h"
#include <future>
#include <raylib.h>
#include <utility>
#include <vector>

using std::vector, std::future;

namespace rt {
  struct AsyncRenderData {
    vector<Pixel>        pixelJobs;
    vector<future<void>> threads;
    Scene                currScene;
    vector<long>         threadTimes;
    vector<int>          threadProgress;
    vector<int>          threadShouldRun;
    vector<bool>         finishedThreads;
    RenderTexture2D      incrementalBuffer;
    RenderTexture2D      screenBuffer;
    bool                 incRender;
  };
} // namespace rt
