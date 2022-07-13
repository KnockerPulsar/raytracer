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
    vector<sPtr<thread>>  threads;

    sPtr<JobQueue<Pixel>> pixelJobs;

    vector<long>          threadTimes;
    vector<int>           threadProgress;
    vector<bool>          finishedThreads;

    bool                  exit = false; // To make threads exit their loops

    RenderTexture2D raytraceRT;
    RenderTexture2D rasterRT;

  public:
    AsyncRenderData() = default;

    AsyncRenderData(int imageWidth, int imageHeight, int numThreads)
        : threadProgress(vector(numThreads, 0)), threadTimes(vector(numThreads, 0L)),
          finishedThreads(vector(numThreads, false)) {

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

    void KillThreads() {
      this->pixelJobs->awakeAllWorkers();

      // Tell threads to exit
      this->exit = true;

      // Join threads
      for (auto &&t : this->threads) {
        t->join();
      }

      // Clear thread vector
      this->threads.clear();
    }

    void changeNumThreads(int newNumThreads) {
      KillThreads();
      threadProgress.resize(newNumThreads);
      threadTimes.resize(newNumThreads);
      finishedThreads.resize(newNumThreads);
    }

    ~AsyncRenderData() { KillThreads(); }
  };
} // namespace rt
