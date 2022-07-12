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
    vector<long>          threadTimes;
    vector<int>           threadProgress;
    bool                  exit = false; // To make threads exit their loops
    vector<bool>          finishedThreads;

    RenderTexture2D raytraceRT;
    RenderTexture2D rasterRT;

  public:
    AsyncRenderData() = default;

    AsyncRenderData(int imageWidth, int imageHeight)
        : threadProgress(vector(NUM_THREADS, 0)), threadTimes(vector(NUM_THREADS, 0L)),
          finishedThreads(vector(NUM_THREADS, false)) {

      int queueChunkSize = 1024;
      pixelJobs          = std::make_shared<JobQueue<Pixel>>(imageWidth * imageHeight, queueChunkSize);

      raytraceRT = LoadRenderTexture(imageWidth, imageHeight);
      rasterRT   = LoadRenderTexture(imageWidth, imageHeight);

      // Prepare pixel jobs
      for (int y = 0; y < imageHeight; y++) {
        for (int x = 0; x < imageWidth; x++) {
          pixelJobs->addJobNoLock(Pixel{x, y, vec3::Zero()});
        }
      }
    }

    AsyncRenderData(sPtr<JobQueue<Pixel>> pj, vector<long> tt, vector<int> tp, vector<bool> ft)
        : pixelJobs(pj), threadTimes(tt), threadProgress(tp), finishedThreads(ft) {}
  };

} // namespace rt
