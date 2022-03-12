#pragma once
#include <iostream>
#include "Pixel.h"
#include "Scene.h"
#include <future>
#include <raylib.h>
#include <utility>
#include <vector>

using std::vector, std::future;

namespace raytracer {
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
} // namespace raytracer