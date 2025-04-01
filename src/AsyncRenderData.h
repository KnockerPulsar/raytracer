#pragma once
#include "Defs.h"
#include "data_structures/Pixel.h"

#include <raylib.h>

#include <thread>
#include <vector>

namespace rt {

  template<typename JobData> class JobQueue;

  struct AsyncRenderData {
    std::vector<sPtr<std::thread>> threads;

    sPtr<JobQueue<Pixel>> pixelJobs;

    std::vector<long> threadTimes;
    std::vector<int>  threadProgress;
    std::vector<bool> finishedThreads;

    bool exit = false; // To make threads exit their loops

    RenderTexture2D raytraceRT;

  public:
    AsyncRenderData() = default;

    AsyncRenderData(int imageWidth, int imageHeight, int editorWidth,
                    int editorHeight, int numThreads);

    AsyncRenderData(sPtr<JobQueue<Pixel>> pj, std::vector<long> tt, std::vector<int> tp, std::vector<bool> ft)
        : pixelJobs(pj), threadTimes(tt), threadProgress(tp), finishedThreads(ft) {}

    void KillThreads();

    void changeNumThreads(int newNumThreads);

    ~AsyncRenderData() { KillThreads(); }
  };
} // namespace rt
