#pragma once
#include "Defs.h"
#include "Scene.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"
#include <future>
#include <iostream>
#include <raylib.h>
#include <utility>
#include <vector>

using std::vector, std::thread;

namespace rt {
  struct AsyncRenderData {
    sPtr<JobQueue<Pixel>> pixelJobs;
    vector<thread>        threads;
    Scene                 currScene;
    vector<long>          threadTimes;
    vector<int>           threadProgress;
    bool                  exit;
    vector<bool>          finishedThreads;
    RenderTexture2D       raytraceRT;
    RenderTexture2D       rasterRT;
    bool                  incRender;
  };
} // namespace rt
