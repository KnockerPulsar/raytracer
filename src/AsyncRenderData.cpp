#include "AsyncRenderData.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"


namespace rt {
AsyncRenderData::AsyncRenderData(int imageWidth, int imageHeight,
                                 int editorWidth, int editorHeight,
                                 int numThreads)
    : threadProgress(vector(numThreads, 0)),
      threadTimes(vector(numThreads, 0L)),
      finishedThreads(vector(numThreads, false)) {

  int queueChunkSize = imageWidth * 4;
  pixelJobs = std::make_shared<JobQueue<Pixel>>(imageWidth * imageHeight,
                                                queueChunkSize);

  rasterRT = LoadRenderTexture(editorWidth, editorHeight);
  raytraceRT = LoadRenderTexture(imageWidth, imageHeight);

  // Prepare pixel jobs
  for (int y = 0; y < imageHeight; y++) {
    for (int x = 0; x < imageWidth; x++) {
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
