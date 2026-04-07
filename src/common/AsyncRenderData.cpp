#include "AsyncRenderData.h"

#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"

namespace rt::raytracer {
  // TODO tiles instead of lines?
  // Every thread gets 4 lines of the output
  AsyncRenderData::AsyncRenderData(Config &&config)
      : threadProgress(config.numThreads, 0), threadTimes(config.numThreads, 0L),
        finishedThreads(0),
        pixelJobs(config.imageSize.x * config.imageSize.y, config.imageSize.x * 4,
                  [this](int threadIndex) { finishedThreads++; }),
        config(std::move(config)) {

    raytraceRT = LoadRenderTexture(config.imageSize.x, config.imageSize.y);

    // Prepare pixel jobs
    for (int y = 0; y < config.imageSize.y; y++) {
      for (int x = 0; x < config.imageSize.x; x++) {
        pixelJobs.addJobNoLock(Pixel{x, y, vec3::Zero()});
      }
    }
  }

  AsyncRenderData::~AsyncRenderData() {
    KillThreads();
    UnloadRenderTexture(raytraceRT);
  }

  void AsyncRenderData::KillThreads() {
    this->pixelJobs.awakeAllWorkers();

    // Tell threads to exit
    this->exit = true;

    // Join threads
    for (auto &&t : this->threads) {
      t.join();
    }

    // Clear thread vector
    this->threads.clear();
  }
} // namespace rt::raytracer
