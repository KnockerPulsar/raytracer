#include <algorithm>
#include <array>
#include <exception>
#include <future>
#include <iostream>
#include <raylib.h>
#include <tuple>
#include <utility>
#include <vector>

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
#define NUM_THREADS 12

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

// bool -> done or not
// future -> data returned from async
using RunningJob = std::pair<bool, future<void>>;
using Workers = vector<RunningJob>;

void RenderAsync(vector<Pixel> &pixelJobs, raytracer::Scene &currScene,
                 Workers &threads, vector<long> &threadTime,
                 vector<int> &threadProgress) {

  for (int t = 0; t < NUM_THREADS; t++) {
    int totalJobs = pixelJobs.size();
    int jobsStart = t * totalJobs / NUM_THREADS;
    int jobsEnd   = (t + 1) * totalJobs / NUM_THREADS;

    threads.push_back(make_pair(
        false, async(launch::async, raytracer::Ray::Trace, ref(pixelJobs),
                     jobsStart, jobsEnd, ref(currScene), ref(threadTime[t]),
                     ref(threadProgress[t]))));
  }
}

// Need to pass Workers by reference since we can't return a copy of something
// that contains a future.
auto PrepareAsync(int imageWidth, int imageHeight, Workers &threads) {
  vector<int>  threadProgress(NUM_THREADS, 0);
  vector<long> threadTime(NUM_THREADS, 0);

  vector<Pixel> pixelJobs =
      vector(imageWidth * imageHeight, Pixel{0, 0, Vec3::Zero()});

  // Prepare pixel jobs
  for (int y = 0; y < imageHeight; y++) {
    for (int x = 0; x < imageWidth; x++) {
      pixelJobs[x + imageWidth * y] = Pixel{x, y, Vec3::Zero()};
    }
  }

  return std::make_tuple(pixelJobs, threadTime, threadProgress);
}

bool CheckAsyncProgress(Workers &threads, const vector<int> &threadProgress) {
  std::cout << "\r";
  bool allFinished = true;
  for (int i = 0; i < NUM_THREADS; i++) {
    auto finished = threads[i].second.wait_for(milliseconds());

    if (finished == std::future_status::ready && threads[i].first == false)
      threads[i].first = true;

    allFinished &= threads[i].first;

    std::cout << std::to_string(threadProgress[i]) << "\t";
  }

  return allFinished;
}

void BlitToScreen(vector<Pixel> &pixelJobs, Workers &threads, int imageHeight) {
  threads.clear();

  BeginDrawing();
  ClearBackground(MAGENTA);

  for (auto &&pixel : pixelJobs) {
    Clr clr     = Clr::FromFloat(pixel.color.x, pixel.color.y, pixel.color.z);
    pixel.color = Vec3::Zero();

    // This is siRayColornce raylib starts the vertical axis at the top left
    // While the tutorial assumes it on the bottom right
    int raylibY = imageHeight - pixel.y - 1;
    DrawPixel(pixel.x, raylibY, clr);
  }
  EndDrawing();
}

int main() {
  // Rendering constants for easy modifications.
  const int   imageWidth      = 400;
  const float aspectRatio     = 16.0 / 9.0;
  const int   imageHeight     = (imageWidth / aspectRatio);
  const int   samplesPerPixel = 20;
  const int   maxDepth        = 10;
  bool        fullscreen      = false;

  InitWindow(imageWidth, imageHeight, title);
  SetTargetFPS(60); // Not like we're gonna hit it...

  // Create scene and update required data for rendering.
  raytracer::Scene currScene =
      raytracer::Scene::RandomMovingSpheres(aspectRatio, 11, 11)
          .UpdateRenderData(maxDepth, imageWidth, imageHeight, samplesPerPixel);

  // Prepares the pixel jobs, thread progress and time lists.
  Workers threads;
  auto [pixelJobs, threadTime, threadProgress] =
      PrepareAsync(imageWidth, imageHeight, threads);

  if (fullscreen)
    ToggleFullscreen();

  while (!WindowShouldClose()) {

    // Check for input for WindowShouldClose and to toggle fullscreen.
    PollInputEvents();
    if (IsKeyPressed(KEY_F)) {
      fullscreen = !fullscreen;
      ToggleFullscreen();
    }

    // Update camera and start async again if last frame is done
    if (threads.empty()) {
      // if (currScene.cam.lookFrom.x < -20 || currScene.cam.lookFrom.x > 20)
      //   currScene.cam.moveDir *= -1;

      // currScene.cam.Fwd(GetFrameTime());

      RenderAsync(pixelJobs, currScene, threads, threadTime, threadProgress);
    }

    // Check on thread progress.
    bool allFinished = CheckAsyncProgress(threads, threadProgress);

    // Draw to screen and reset thread jobs.
    if (allFinished) {
      BlitToScreen(pixelJobs, threads, imageHeight);
    }
  }

  // Note that if you kill the application in fullscreen, the resolution won't
  // reset to native.
  if (fullscreen)
    ToggleFullscreen();

  // Clean up.
  CloseWindow();
  TraceLog(LOG_INFO, "Shutting down, waiting for threads to fall asleep. This "
                     "can take some time...");

  for (auto &thread : threads) {
    thread.second.wait();
  }

  return 0;
}
