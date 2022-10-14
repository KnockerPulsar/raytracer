#pragma once

#include "RenderAsync.h"
#include "Scene.h"
#include <condition_variable>
#include <future>
#include <iostream>
#include <raylib.h>
#include <utility>
#include <vector>

using std::vector, std::thread;

namespace rt {

  template<typename JobData> class JobQueue;

  struct AsyncRenderData {
    vector<sPtr<thread>> threads;

    sPtr<JobQueue<Pixel>> pixelJobs;

    vector<float>  threadProgress;
    vector<bool> finishedThreads;

    bool exit = false; // To make threads exit their loops

    Texture2D raytraceRT;
    RenderTexture2D rasterRT;

  public:
    AsyncRenderData() = default;
    AsyncRenderData(int numThreads);
    AsyncRenderData(sPtr<JobQueue<Pixel>> pj, vector<float> tp, vector<bool> ft)
        : pixelJobs(pj), threadProgress(tp), finishedThreads(ft) {}

    void KillThreads();
    void changeNumThreads(int newNumThreads);
    void changeRTResolution(int w, int h);

    ~AsyncRenderData() { KillThreads(); }
  };
} // namespace rt
