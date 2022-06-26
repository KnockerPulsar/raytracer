#include "RenderAsync.h"
#include "Constants.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <memory>
#include <numeric>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "../vendor/rlImGui/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "AsyncRenderData.h"
#include "Defs.h"
#include "Ray.h"
#include "Scene.h"
#include "data_structures/Pixel.h"

using std::pair, std::future, std::vector, std::async;

using rt::Hittable, rt::Pixel;

using std::string, std::vector, std::chrono::high_resolution_clock, std::chrono::duration_cast,
    std::chrono::milliseconds, std::pair, std::future, std::future, std::async, std::ref, std::make_pair, std::launch,
    std::future, std::async, std::ref, std::make_pair, std::launch;

namespace rt {
  pair<int, int> RenderAsync::GetThreadJobSlice(int totalJobs, int t) {
    int jobsStart = t * totalJobs / NUM_THREADS;
    int jobsEnd   = (t + 1) * totalJobs / NUM_THREADS;
    return {jobsStart, jobsEnd};
  }

  AsyncRenderData RenderAsync::Perpare(int imageWidth, float aspectRatio, int maxDepth, int samplesPerPixel) {
    int imageHeight = imageWidth / aspectRatio;

    vector<int>           threadProgress(NUM_THREADS, 0);
    vector<long>          threadTimes(NUM_THREADS, 0);
    vector<int>           threadShouldRun(NUM_THREADS, 1); // Used to exit early
    vector<bool>          finishedThreads(NUM_THREADS, false);
    sPtr<JobQueue<Pixel>> pixelJobs = std::make_shared<JobQueue<Pixel>>(imageWidth * imageHeight, 1024);

    // Prepare pixel jobs
    for (int y = 0; y < imageHeight; y++) {
      for (int x = 0; x < imageWidth; x++) {
        pixelJobs->addJobNoLock(Pixel{x, y, vec3::Zero()});
      }
    }

    return AsyncRenderData(pixelJobs, threadTimes, threadProgress, finishedThreads);
  }

  void RenderAsync::Start(AsyncRenderData &ard) {

    ard.exit = false;
    // For some reason, incRender gets set to 129 here
    // Need to set it back to it's original value
    for (int t = 0; t < NUM_THREADS; t++) {
      ard.threads.push_back(std::make_shared<std::thread>(rt::Ray::Trace, ref(ard), t));
    }
  }

  void RenderAsync::BlitToBuffer(vector<Pixel> &pixelJobs, int drawStart, int drawEnd, RenderTexture2D &screenBuffer) {

    BeginTextureMode(screenBuffer);

    for (int i = drawStart; i < drawEnd; i++) {
      Pixel &pixel = pixelJobs[i];

      // Clamp r, g, and b to prevent underflows and artifacts

      Color clr = ColorFromFloats(Clamp(pixel.color.x, 0, 1), Clamp(pixel.color.y, 0, 1), Clamp(pixel.color.z, 0, 1));

      pixel.color = vec3::Zero();

      DrawPixel(pixel.x, pixel.y, clr);
    }
    EndTextureMode();
  }

  bool RenderAsync::RenderFinished(AsyncRenderData &ard, bool &allFinished) {
    if (ard.pixelJobs->jobsDone() && !allFinished) {
      allFinished = true;
      BlitToBuffer(ard.pixelJobs->getJobsVector(), 0, ard.pixelJobs->numberOfJobs, ard.raytraceRT);
    }

    if (allFinished) {
      ClearBackground(BLACK);
      DrawTextureRec(ard.raytraceRT.texture,
                     (Rectangle){0, 0, (float)ard.currScene->imageWidth, (float)ard.currScene->imageWidth},
                     (Vector2){0, 0},
                     WHITE);
    }

    return allFinished;
  }

  void RenderAsync::Shutdown(AsyncRenderData &ard) {
    ard.pixelJobs->awakeAllWorkers();

    // Tell threads to exit
    ard.exit = true;

    // Join threads
    for (auto &&t : ard.threads) {
      t->join();
    }

    // Clear thread vector
    ard.threads.clear();

    // Reset job queue chunks
    ard.pixelJobs->setCurrentChunkStart(0);

    // Reset thread times and progress
    for (int i = 0; i < NUM_THREADS; i++) {
      ard.threadTimes[i]    = 0;
      ard.threadProgress[i] = 0;
    }

    // Clear results from previous job.
    for (auto it = ard.pixelJobs->getJobsVector().begin(); it != ard.pixelJobs->getJobsVector().end(); ++it) {
      it->color = vec3(0);
    }
  }

  void RenderAsync::RenderImGui(bool showProg, AsyncRenderData &ard) {
    rlImGuiBegin();

    if (showProg) {
      if (ImGui::Begin("Thread status", 0)) {
        ImGui::Text("%s", "Press space to toggle this menu");
        ImGui::Text("%s", "Press escape to quit");
        // ImGui::Checkbox("Incremental rendering", &ard.incRender);

        ImGui::ProgressBar(float(ard.pixelJobs->getCurrentChunkStart()) / ard.pixelJobs->getJobsVector().size());
        ImGui::Text("Rendering progress");

        if (ImGui::BeginTable("Thread status", 3)) {
          ImGui::TableNextRow();

          for (int t = 0; t < NUM_THREADS; t++) {
            // Thread labels
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Thread %d: ", t);
            ImGui::TableNextColumn();
            ImGui::ProgressBar(ard.threadProgress[t] / 100.0f);
            ImGui::TableNextColumn();
            ImGui::Text("Time: %ld ms", ard.threadTimes[t]);
          }

          ImGui::EndTable();
        }
        ImGui::End();
      }
    }

    rlImGuiEnd();
  }

  void RenderAsync::ResetThreads(AsyncRenderData &ard) { ard.pixelJobs->awakeAllWorkers(); }
} // namespace rt