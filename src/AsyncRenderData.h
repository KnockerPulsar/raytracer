#pragma once
#include "Defs.h"
#include "RenderAsync.h"
#include "Scene.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"
#include <condition_variable>
#include <future>
#include <iostream>
#include <raylib.h>
#include <utility>
#include <vector>

using std::vector, std::thread;

namespace rt {
  struct AsyncRenderData {
    sPtr<JobQueue<Pixel>> pixelJobs;
    vector<sPtr<thread>>  threads;
    sPtr<Scene>           currScene;
    vector<long>          threadTimes;
    vector<int>           threadProgress;
    bool                  exit = false; // To make threads exit their loops
    vector<bool>          finishedThreads;

    RenderTexture2D raytraceRT;
    RenderTexture2D rasterRT;

  public:
    AsyncRenderData() = default;

    AsyncRenderData(sPtr<JobQueue<Pixel>> pj, vector<long> tt, vector<int> tp, vector<bool> ft)
        : pixelJobs(pj), threadTimes(tt), threadProgress(tp), finishedThreads(ft) {}

    AsyncRenderData &setScene(sPtr<Scene> s) {
      currScene = s;
      return *this;
    }

    ~AsyncRenderData() { RenderAsync::Shutdown(*this); }
  };

} // namespace rt
