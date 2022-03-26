#include "RenderAsync.h"
#include "Constants.h"
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
#include "AsyncRenderData.h"
#include "Camera.h"
#include "Constants.h"
#include "Ray.h"
#include "Scene.h"
#include "data_structures/Pixel.h"

using std::pair, std::future, std::vector, std::async;

using raytracer::Hittable, raytracer::Pixel;

using std::string, std::vector, std::chrono::high_resolution_clock,
    std::chrono::duration_cast, std::chrono::milliseconds, std::pair,
    std::future, std::future, std::async, std::ref, std::make_pair, std::launch,
    std::future, std::async, std::ref, std::make_pair, std::launch;

namespace raytracer {
  pair<int, int> RenderAsync::GetThreadJobSlice(int totalJobs, int t) {
    int jobsStart = t * totalJobs / NUM_THREADS;
    int jobsEnd   = (t + 1) * totalJobs / NUM_THREADS;
    return {jobsStart, jobsEnd};
  }

  AsyncRenderData RenderAsync::Perpare(int imageWidth, float aspectRatio,
                                       int maxDepth, int samplesPerPixel,
                                       SceneID sceneID, int incRender,
                                       int gridWidth, int gridHeight) {

    int imageHeight = imageWidth / aspectRatio;

    InitWindow(imageWidth, imageHeight, title.c_str());
    rlImGuiSetup(true);
    SetTargetFPS(60); // Not like we're gonna hit it...

    RenderTexture2D screenBuffer = LoadRenderTexture(imageWidth, imageHeight);
    RenderTexture2D incBuffer    = LoadRenderTexture(imageWidth, imageHeight);

    // Scene currScene;

    // if (sceneID == SceneID::scene1)
    //   currScene =
    //       Scene::Scene1(imageWidth, imageHeight, maxDepth, samplesPerPixel);
    // else if (sceneID == SceneID::scene2)
    //   currScene =
    //       Scene::Scene2(imageWidth, imageHeight, maxDepth, samplesPerPixel);
    // else if (sceneID == SceneID::random)
    //   currScene =
    //       Scene::Random(imageWidth, imageHeight, maxDepth, samplesPerPixel);
    // else if (sceneID == SceneID::random_moving)
    //   currScene = Scene::RandomMovingSpheres(imageWidth,
    //                                          imageHeight,
    //                                          maxDepth,
    //                                          samplesPerPixel,
    //                                          gridWidth,
    //                                          gridHeight);
    // else if (sceneID == SceneID::two_spheres)
    //   currScene =
    //       Scene::TwoSpheres(imageWidth, imageHeight, maxDepth,
    //       samplesPerPixel);
    // else if (sceneID == SceneID::earth)
    //   currScene =
    //       Scene::Earth(imageWidth, imageHeight, maxDepth, samplesPerPixel);
    // else if (sceneID == SceneID::light)
    //   currScene =
    //       Scene::Light(imageWidth, imageHeight, maxDepth, samplesPerPixel);
    // else if (sceneID == SceneID::cornell) {
    //   currScene =
    //       Scene::CornellBox(imageWidth, imageHeight, maxDepth,
    //       samplesPerPixel);
    // }

    vector<future<void>> threads;
    vector<int>          threadProgress(NUM_THREADS, 0);
    vector<long>         threadTimes(NUM_THREADS, 0);
    vector<int>          threadShouldRun(NUM_THREADS, 1); // Used to exit early
    vector<bool>         finishedThreads(NUM_THREADS, false);
    vector<Pixel>        pixelJobs =
        vector(imageWidth * imageHeight, Pixel{0, 0, Vec3::Zero()});

    // Prepare pixel jobs
    for (int y = 0; y < imageHeight; y++) {
      for (int x = 0; x < imageWidth; x++) {
        pixelJobs[x + imageWidth * y] = Pixel{x, y, Vec3::Zero()};
      }
    }

    return (AsyncRenderData){pixelJobs,
                             // Must move since futures can only have one owner
                             std::move(threads),
                             Scene(),
                             threadTimes,
                             threadProgress,
                             threadShouldRun,
                             finishedThreads,
                             incBuffer,
                             screenBuffer,
                             (bool)incRender};
  }

  void RenderAsync::Start(AsyncRenderData &ard) {

    // For some reason, incRender gets set to 129 here
    // Need to set it back to it's original value
    for (int t = 0; t < NUM_THREADS; t++) {
      auto [jobsStart, jobsEnd] = GetThreadJobSlice(ard.pixelJobs.size(), t);

      ard.threads.push_back(async(launch::async,
                                  raytracer::Ray::Trace,
                                  ref(ard.pixelJobs),
                                  jobsStart,
                                  jobsEnd,
                                  ref(ard.currScene),
                                  ref(ard.threadTimes[t]),
                                  ref(ard.threadProgress[t]),
                                  ref(ard.threadShouldRun[t])));
    }
  }

  void RenderAsync::BlitToBuffer(vector<Pixel> &pixelJobs, int drawStart,
                                 int drawEnd, RenderTexture2D &screenBuffer) {

    BeginTextureMode(screenBuffer);

    for (int i = drawStart; i < drawEnd; i++) {
      Pixel &pixel = pixelJobs[i];

      // Clamp r, g, and b to prevent underflows and artifacts

      Color clr = ColorFromFloats(Clamp(pixel.color.x, 0, 1),
                                  Clamp(pixel.color.y, 0, 1),
                                  Clamp(pixel.color.z, 0, 1));

      pixel.color = Vec3::Zero();

      DrawPixel(pixel.x, pixel.y, clr);
    }
    EndTextureMode();
  }

  bool RenderAsync::RenderFinished(AsyncRenderData &ard) {
    bool allFinished = true;
    for (int i = 0; i < NUM_THREADS; i++) {
      auto finished = ard.threads[i].wait_for(milliseconds(0));

      if (finished == std::future_status::ready &&
          ard.finishedThreads[i] == false) {
        ard.finishedThreads[i] = true;
        if (ard.incRender) {
          auto [jobsStart, jobsEnd] =
              GetThreadJobSlice(ard.pixelJobs.size(), i);
          BlitToBuffer(
              ard.pixelJobs, jobsStart, jobsEnd, ard.incrementalBuffer);
        }
      }

      allFinished &= ard.finishedThreads[i];
    }

    if (!ard.incRender && allFinished) {
      BlitToBuffer(ard.pixelJobs, 0, ard.pixelJobs.size(), ard.screenBuffer);
    }

    ClearBackground(BLACK);

    // If we're rendering incrementally
    // Or we're not rendering incrementally but all threads finished
    // Render to screen
    Texture2D &currentTex = ard.incRender ? ard.incrementalBuffer.texture
                                          : ard.screenBuffer.texture;

    DrawTextureRec(currentTex,
                   (Rectangle){0,
                               0,
                               (float)ard.currScene.imageWidth,
                               (float)ard.currScene.imageWidth},
                   (Vector2){0, 0},
                   WHITE);

    return allFinished;
  }

  void RenderAsync::Shutdown(bool fullscreen, AsyncRenderData &ard) {

    // Note that if you kill the application in fullscreen, the resolution won't
    // reset to native.
    if (fullscreen)
      ToggleFullscreen();

    // Clean up.

    string shutdownMessage = "Shutting down, please wait...";

    bool allFinished;
    int  textSize = 80 * ard.currScene.imageWidth / 1080.0;

    std::for_each(ard.threadShouldRun.begin(),
                  ard.threadShouldRun.end(),
                  [&](int &shouldRun) { shouldRun = 0; });

    do {

      allFinished = true;
      for (int i = 0; i < ard.threads.size(); i++) {
        auto threadStatus = ard.threads[i].wait_for(milliseconds(0));

        allFinished &= (threadStatus == std::future_status::ready);
      }

      BeginDrawing();
      int textWidth = MeasureText(shutdownMessage.c_str(), textSize);
      ClearBackground(BLACK);
      DrawText(shutdownMessage.c_str(),
               ard.currScene.imageWidth / 2.0 - textWidth / 2.0,
               ard.currScene.imageHeight / 2.0,
               textSize,
               WHITE);
      EndDrawing();
    } while (!allFinished);

    UnloadRenderTexture(ard.incrementalBuffer);
    rlImGuiShutdown();
    CloseWindow();
  }
  void RenderAsync::RenderImGui(bool showProg, AsyncRenderData &ard) {
    rlImGuiBegin();

    if (showProg) {
      if (ImGui::Begin("Thread status"), 0, ImGuiWindowFlags_AlwaysAutoResize) {
        ImGui::Text("%s", "Press space to toggle this menu");
        ImGui::Text("%s", "Press escape to quit");
        ImGui::Checkbox("Incremental rendering", &ard.incRender);

        if (ImGui::BeginTable("Thread status", 3)) {
          ImGui::TableNextRow();

          for (int t = 0; t < NUM_THREADS; t++) {
            // Thread labels
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Thread %d progress", t);
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

  void RenderAsync::ResetThreads(AsyncRenderData &ard) {
    ard.threads.clear();
    std::fill(ard.finishedThreads.begin(), ard.finishedThreads.end(), false);

    if (ard.currScene.cam.lookFrom.x < -20 || ard.currScene.cam.lookFrom.x > 20)
      ard.currScene.cam.moveDir *= -1;

    ard.currScene.cam.Fwd(GetFrameTime());

    RenderAsync::Start(ard);
  }

  void RenderAsync::CheckInput(bool &fullscreen, bool &showProg) {
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
  }
} // namespace raytracer