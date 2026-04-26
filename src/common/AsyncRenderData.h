#pragma once

#include "data_structures/Pixel.h"
#include "data_structures/JobQueue.h"
#include "raytracer/config.h"

#include <raylib.h>

#include <thread>
#include <vector>

// TODO should be moved to `src/raytracer`
namespace rt::raytracer {
  class Config;

  struct AsyncRenderData {
    std::vector<std::thread> threads;

    JobQueue<Pixel> pixelJobs;

    std::vector<long> threadTimes;
    std::vector<int>  threadProgress;
    std::atomic<int>  finishedThreads;

    bool exit = false; // To make threads exit their loops

  public:
    AsyncRenderData(Config &&config);
    ~AsyncRenderData();

    void KillThreads();
    float progress() const;
    bool isRaytracing() const;
    bool finishedRaytracing() const;

    Config const config;
  };
} // namespace rt
