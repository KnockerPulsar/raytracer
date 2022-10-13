#include "AsyncRenderData.h"
#include "Constants.h"
#include "IState.h"
#include "Ray.h"
#include "RenderAsync.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"
#include "imgui.h"
#include "stb_image_write.h"
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <raylib.h>
#include <rlImGui.h>
#include <sstream>
#include "BVHNode.h"

using std::ref;

namespace rt {
  class Raytracer : public IState {
    bool allFinished = false;
    float renderTime = 0;

  public:
    bool             showProg = true;
    AsyncRenderData &ard;

    Raytracer(AsyncRenderData &ard) : ard(ard) {}

    void prepass() {
      for (auto& obj : getScene()->worldRoot->getChildrenAsList()) {
        obj->transformation.constructMatrices();
      }
    }

    virtual void onEnter() {
      prepass();
      startRaytracing();
    }

    virtual void onExit() {
      ard.KillThreads();

      // Reset job queue chunks
      ard.pixelJobs->setCurrentChunkStart(0);

      // Reset thread times and progress
      for (int i = 0; i < app->getNumThreads(); i++) {
        ard.threadProgress[i]  = 0;
        ard.finishedThreads[i] = false;
      }

      // Clear results from previous job.
      for (auto it = ard.pixelJobs->getJobsVector().begin(); it != ard.pixelJobs->getJobsVector().end(); ++it) {
        it->color = vec3(0);
      }

      allFinished = false;
    }

    virtual void onUpdate() {
      if(!allFinished)
        renderTime += GetFrameTime();

      BeginDrawing();

      onFinished();

      BlitToBuffer();

      DrawTextureRec(
          ard.raytraceRT.texture,
          (Rectangle){0, 0, (float)getScene()->imageWidth, -(float)getScene()->imageHeight},
          (Vector2){0, 0},
          WHITE
      );

      RenderImGui();

      EndDrawing();
    }

    void startRaytracing() {
      ard.exit = false;

      for (auto &job : ard.pixelJobs->getJobsVector()) {
        job.color = vec3::Zero();
      }

      for (int t = 0; t < app->getNumThreads(); t++) {
        ard.threads.push_back(std::make_shared<std::thread>(Ray::Trace, ref(ard), getScene(), t));
      }

      renderTime = 0;

      std::cout << "Raytracing started! (" << app->getNumThreads() << ")\n";
    }

    void BlitToBuffer() {
      static bool firstFrame = true;

      auto* pixelData = new vec3[getScene()->imageWidth * getScene()->imageHeight];
      auto jobs      = ard.pixelJobs->getJobsVector();

      // Copy over only the color data
      for (int i = 0; i < jobs.size(); ++i) {
        pixelData[i] = jobs[i].color;
      }

      if (firstFrame) {
        // Create texture from image containing the color data
        Image raytraced = {
            .data    = pixelData,
            .width   = getScene()->imageWidth,
            .height  = getScene()->imageHeight,
            .mipmaps = 1,
            .format  = RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32
          };

        ard.raytraceRT.texture = LoadTextureFromImage(raytraced);

        UnloadImage(raytraced);
        firstFrame = false;
      } else {
        UpdateTexture(ard.raytraceRT.texture, pixelData);

        delete[] pixelData;
      }

    }

    bool onFinished() {
      ClearBackground(BLACK);

      bool finished = true;
      for (auto finishedThread : ard.finishedThreads) {
        finished &= finishedThread;
      }

      if (finished && !allFinished) {
        allFinished = true;

        ard.KillThreads();

        if (app->saveOnRender)
          Autosave();
      }
      // printf("%s\n", allFinished ? "all done" : "not yet");
      return allFinished;
    }

    void RenderImGui() {

      rlImGuiBegin();

      if (showProg) {
        if (ImGui::Begin("Thread status", 0)) {
          ImGui::Text("%s", "Press space to toggle this menu");
          ImGui::Text("%s", "Press escape to quit");

          ImGui::Spacing();

          ImGui::Text("Render time: %f seconds", renderTime);

          ImGui::Spacing();

          ImGui::Text("Rendering progress");
          ImGui::ProgressBar(allFinished? 1.0f : float(ard.pixelJobs->getCurrentChunkStart()) / ard.pixelJobs->getJobsVector().size());

          // std::cout << "Progress: " << float(ard.pixelJobs->getCurrentChunkStart()) / ard.pixelJobs->getJobsVector().size() << "\n";

          ImGui::Spacing();

          if (ImGui::BeginTable("Thread status", 3)) {
            ImGui::TableNextRow();

            for (int t = 0; t < app->getNumThreads(); t++) {
              // Thread labels
              ImGui::TableNextRow();
              ImGui::TableNextColumn();
              ImGui::Text("Thread %d: ", t);
              ImGui::TableNextColumn();
              ImGui::ProgressBar(allFinished ? 1.0f : ard.threadProgress[t]);
              ImGui::TableNextColumn();
            }

            ImGui::EndTable();
          }
          ImGui::End();
        }
      }

      rlImGuiEnd();
    }

    void Autosave() {
      std::stringstream ss;

      auto t  = std::time(nullptr);
      auto tm = *std::localtime(&t);
      ss << "screenshots/" << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "_" << getScene()->imageWidth << "x"
         << getScene()->imageHeight << "_" << getScene()->settings.samplesPerPixel << "_"
         << getScene()->settings.maxDepth << ".bmp";

      std::cout << "Finished render: " << ss.str() << std::endl;

      Image raytraced = LoadImageFromTexture(ard.raytraceRT.texture);

      // 4 components for RGBA;
      // Couldn't get raylibs ExportImage to work with BMP. It did work with PNG however
      stbi_write_bmp(ss.str().c_str(), raytraced.width, raytraced.height, 4, raytraced.data);

      UnloadImage(raytraced);
    }
  };
} // namespace rt