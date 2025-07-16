#include "AsyncRenderData.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"


namespace rt {
  AsyncRenderData::AsyncRenderData(glm::ivec2 imageSize, glm::ivec2 editorSize, int numThreads)
      : threadProgress(std::vector(numThreads, 0)), threadTimes(std::vector(numThreads, 0L)),
        finishedThreads(std::vector(numThreads, false)) {

    int queueChunkSize = imageSize.x * 4;
    pixelJobs          = std::make_shared<JobQueue<Pixel>>(imageSize.x * imageSize.y, queueChunkSize);

    raytraceRT = LoadRenderTexture(imageSize.x, imageSize.y);

    // Prepare pixel jobs
    for (int y = 0; y < imageSize.y; y++) {
      for (int x = 0; x < imageSize.x; x++) {
        pixelJobs->addJobNoLock(Pixel{x, y, vec3::Zero()});
      }
    }
  }

  void AsyncRenderData::KillThreads() {
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

  void AsyncRenderData::changeNumThreads(int newNumThreads) {
    KillThreads();
    threadProgress.resize(newNumThreads);
    threadTimes.resize(newNumThreads);
    finishedThreads.resize(newNumThreads);
  }
} // namespace rt
