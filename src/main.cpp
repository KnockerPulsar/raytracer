#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <numeric>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "../vendor/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "Camera.h"
#include "Clr.h"
#include "Constants.h"
#include "Pixel.h"
#include "Scene.h"

using raytracer::Hittable, raytracer::Clr, raytracer::Pixel;

using std::string, std::vector, std::chrono::high_resolution_clock,
    std::chrono::duration_cast, std::chrono::milliseconds, std::pair,
    std::future, std::future, std::async, std::ref, std::make_pair, std::launch,
    std::future, std::async, std::ref, std::make_pair, std::launch;

#define title "Raytracer"

// TODO: Check out raygui for debug panels.
// TODO: Separate checking on threads and blitting finished thread pixels.
// TODO: Implement a clearer pipeline (Init, loop{Check inputs, render}, clean
// up).
// TODO: Add progress bar to shutdown loop.
// TODO: Something to switch between incremental and whole frame drawing.

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

// bool -> done or not
// future -> data returned from async
using RunningJob = std::pair<bool, future<void>>;
using Workers    = vector<RunningJob>;

pair<int, int> GetThreadJobSlice(int totalJobs, int t) {
  int jobsStart = t * totalJobs / NUM_THREADS;
  int jobsEnd   = (t + 1) * totalJobs / NUM_THREADS;
  return {jobsStart, jobsEnd};
}

void RenderAsync(vector<Pixel> &pixelJobs, raytracer::Scene &currScene,
                 Workers &threads, vector<long> &threadTime,
                 vector<int> &threadProgress, vector<int> &threadShouldRun) {

  for (int t = 0; t < NUM_THREADS; t++) {
    auto [jobsStart, jobsEnd] = GetThreadJobSlice(pixelJobs.size(), t);

    threads.push_back(make_pair(
        false, async(launch::async, raytracer::Ray::Trace, ref(pixelJobs),
                     jobsStart, jobsEnd, ref(currScene), ref(threadTime[t]),
                     ref(threadProgress[t]), ref(threadShouldRun[t]))));
  }
}

// Need to pass Workers by reference since we can't return a copy of something
// that contains a future.
auto PrepareAsync(int imageWidth, int imageHeight, Workers &threads) {
  vector<int>  threadProgress(NUM_THREADS, 0);
  vector<long> threadTime(NUM_THREADS, 0);

  // Used to signal threads to exit prematurely
  vector<int> threadShouldRun = vector(NUM_THREADS, 1);

  vector<Pixel> pixelJobs =
      vector(imageWidth * imageHeight, Pixel{0, 0, Vec3::Zero()});

  // Prepare pixel jobs
  for (int y = 0; y < imageHeight; y++) {
    for (int x = 0; x < imageWidth; x++) {
      pixelJobs[x + imageWidth * y] = Pixel{x, y, Vec3::Zero()};
    }
  }

  return std::make_tuple(pixelJobs, threadTime, threadProgress,
                         threadShouldRun);
}

void BlitToBuffer(vector<Pixel> &pixelJobs, int drawStart, int drawEnd,
                  RenderTexture2D &screenBuffer) {

  BeginTextureMode(screenBuffer);

  for (int i = drawStart; i < drawEnd; i++) {
    Pixel &pixel   = pixelJobs[i];
    auto [r, g, b] = pixel.color;

    // Clamp r, g, and b to prevent underflows and artifacts
    r = Clamp(r, 0, 1);
    g = Clamp(g, 0, 1);
    b = Clamp(b, 0, 1);

    Clr clr     = Clr::FromFloat(r, g, b);
    pixel.color = Vec3::Zero();

    DrawPixel(pixel.x, pixel.y, clr);
  }
  EndTextureMode();
}

bool CheckAsyncProgress(vector<Pixel> &pixelJobs, RenderTexture2D &screenBuffer,
                        Workers &threads, const vector<int> &threadProgress) {
  // std::cout << "\r";
  bool allFinished = true;
  for (int i = 0; i < NUM_THREADS; i++) {
    auto finished = threads[i].second.wait_for(milliseconds(0));

    if (finished == std::future_status::ready && threads[i].first == false) {
      threads[i].first          = true;
      auto [jobsStart, jobsEnd] = GetThreadJobSlice(pixelJobs.size(), i);
      BlitToBuffer(pixelJobs, jobsStart, jobsEnd, screenBuffer);
    }

    allFinished &= threads[i].first;

    // std::cout << std::to_string(threadProgress[i]) << "\t";
  }

  return allFinished;
}

void PrintFrameTimes(vector<long> &threadTime) {
  static bool first = true;

  if (!first) {
    for (int i = 0; i < NUM_THREADS; i++) {
      std::cout << "\033[A";
    }
  }

  std::cout << '\n';
  for (int i = 0; i < NUM_THREADS; i++) {
    std::cout << "Thread " << i << ":\t " << threadTime[i] << " ms\n";
  }
  first = false;
}

int main() {
  // Rendering constants for easy modifications.
  const int   imageWidth      = 1080;
  const float aspectRatio     = 1;
  const int   imageHeight     = (imageWidth / aspectRatio);
  const int   samplesPerPixel = 100;
  const int   maxDepth        = 8;
  bool        fullscreen      = false;
  bool        showProg        = true;

  InitWindow(imageWidth, imageHeight, title);
  rlImGuiSetup(true);
  SetTargetFPS(60); // Not like we're gonna hit it...
  RenderTexture2D screenBuffer = LoadRenderTexture(imageWidth, imageHeight);

  // Create scene and update required data for rendering.
  raytracer::Scene currScene =
      raytracer::Scene::CornellBox(aspectRatio)
          .UpdateRenderData(maxDepth, imageWidth, imageHeight, samplesPerPixel);

  // Prepares the pixel jobs, thread progress and time lists.
  Workers threads;
  auto [pixelJobs, threadTime, threadProgress, threadShouldRun] =
      PrepareAsync(imageWidth, imageHeight, threads);

  if (fullscreen)
    ToggleFullscreen();

  while (!WindowShouldClose()) {

    int keyPressed = GetKeyPressed();
    switch (keyPressed) {
    case KEY_F: {
      fullscreen = !fullscreen;
      ToggleFullscreen();
    }
    case KEY_SPACE: {
      showProg = !showProg;
    }
    }

    // Update camera and start async again if last frame is done
    if (threads.empty()) {
      if (currScene.cam.lookFrom.x < -20 || currScene.cam.lookFrom.x > 20)
        currScene.cam.moveDir *= -1;

      currScene.cam.Fwd(GetFrameTime());

      RenderAsync(pixelJobs, currScene, threads, threadTime, threadProgress,
                  threadShouldRun);
    }

    // Check on thread progress. Draw finished threads to buffer.
    bool allFinished =
        CheckAsyncProgress(pixelJobs, screenBuffer, threads, threadProgress);

    BeginDrawing();
    rlImGuiBegin();

    ClearBackground(BLACK);

    // Display buffer.
    DrawTextureRec(screenBuffer.texture,
                   (Rectangle){0, 0, imageWidth, imageWidth}, (Vector2){0, 0},
                   WHITE);

    if (showProg) {
      if (ImGui::Begin("Thread status"), 0, ImGuiWindowFlags_AlwaysAutoResize) {
        if (ImGui::BeginTable("Thread status", 3)) {
          ImGui::TableNextRow();

          for (int t = 0; t < NUM_THREADS; t++) {
            // Thread labels
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Thread %d progress", t);
            ImGui::TableNextColumn();
            ImGui::ProgressBar(threadProgress[t] / 100.0f);
            ImGui::TableNextColumn();
            ImGui::Text("Time: %ld ms", threadTime[t]);
          }
          ImGui::EndTable();
        }
        ImGui::End();
      }
    }

    rlImGuiEnd();
    EndDrawing();

    // Draw to screen and reset thread jobs.
    if (allFinished) {
      threads.clear();
    }
  }

  // Note that if you kill the application in fullscreen, the resolution won't
  // reset to native.
  if (fullscreen)
    ToggleFullscreen();

  // Clean up.

  string shutdownMessage = "Shutting down, please wait...";

  bool allFinished;
  int  textSize = 80 * imageWidth / 1080.0;

  std::for_each(threadShouldRun.begin(), threadShouldRun.end(),
                [&](int &shouldRun) { shouldRun = 0; });

  do {

    allFinished = true;
    for (int i = 0; i < threads.size(); i++) {
      auto threadStatus = threads[i].second.wait_for(milliseconds(0));

      allFinished &= (threadStatus == std::future_status::ready);
    }

    BeginDrawing();
    int textWidth = MeasureText(shutdownMessage.c_str(), textSize);
    ClearBackground(BLACK);
    DrawText(shutdownMessage.c_str(), imageWidth / 2.0 - textWidth / 2.0,
             imageHeight / 2.0, textSize, WHITE);
    EndDrawing();
  } while (!allFinished);

  UnloadRenderTexture(screenBuffer);
  rlImGuiShutdown();
  CloseWindow();

  return 0;
}
