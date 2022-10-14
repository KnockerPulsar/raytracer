#include "AsyncRenderData.h"
#include "app.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"
#include <raylib.h>
#include <rlgl.h>


namespace rt {
  AsyncRenderData::AsyncRenderData(int numThreads)
      : threadProgress(vector(numThreads, 0.0f)), 
        finishedThreads(vector(numThreads, false)) {
    changeRTResolution(App::getImageWidth(), App::getImageHeight());
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
    finishedThreads.resize(newNumThreads);
  }

  void AsyncRenderData::changeRTResolution(int w, int h) {
    int queueChunkSize = w * 4;
    pixelJobs          = std::make_shared<JobQueue<Pixel>>(w * h, queueChunkSize);
    pixelJobs->setCurrentChunkStart(0);

    UnloadRenderTexture(rasterRT);
    UnloadTexture(raytraceRT);

    Image rt = GenImageColor(w, h, App::scene.backgroundColor.toRaylibColor(255));
    ImageFormat(&rt, RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32);
    raytraceRT = LoadTextureFromImage(rt);
    UnloadImage(rt);

    rasterRT   = LoadRenderTexture(App::getEditorWidth(), App::getEditorHeight());

    // Prepare pixel jobs
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        pixelJobs->addJobNoLock(Pixel{x, y, vec3::Zero()});
      }
    }
  }
} // namespace rt