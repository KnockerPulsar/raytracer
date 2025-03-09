#pragma once
#include "Defs.h"
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

    vector<long> threadTimes;
    vector<int>  threadProgress;
    vector<bool> finishedThreads;

    bool exit = false; // To make threads exit their loops

    RenderTexture2D raytraceRT;
    RenderTexture2D rasterRT;

  public:
    AsyncRenderData() = default;

    AsyncRenderData(int imageWidth, int imageHeight, int editorWidth,
                    int editorHeight, int numThreads);

    AsyncRenderData(sPtr<JobQueue<Pixel>> pj, vector<long> tt, vector<int> tp, vector<bool> ft)
        : pixelJobs(pj), threadTimes(tt), threadProgress(tp), finishedThreads(ft) {}

    void KillThreads();

    void changeNumThreads(int newNumThreads);

    ~AsyncRenderData() { KillThreads(); }
  };
} // namespace rt
