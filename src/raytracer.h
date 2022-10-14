#pragma once
#include "stb_image_write.h"

#include "AsyncRenderData.h"
#include "Constants.h"
#include "IState.h"
#include "Ray.h"
#include "RenderAsync.h"
#include "app.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"
#include "imgui.h"
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <iterator>
#include <raylib.h>
#include <rlImGui.h>
#include <sstream>
#include <sys/types.h>
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
      for (auto& obj : App::scene.worldRoot->getChildrenAsList()) {
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
      for (int i = 0; i < App::getNumThreads(); i++) {
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

      float edW = App::getEditorWidth();
      float edH = App::getEditorHeight();
      float iW = App::getImageWidth();
      float iH = App::getImageHeight();

      float heightRatio = iH / edH;
      float widthRatio  = iW / edW;

      float ratio        = std::max(heightRatio, widthRatio) * 1.05;
      float scaledWidth  = iW / ratio;
      float scaledHeight = iH / ratio;

      float topLeftX = (edW - scaledWidth) / 2.0f;
      float topLeftY = (edH - scaledHeight) / 2.0f;

      DrawTexturePro(
          ard.raytraceRT.texture, {0, 0, iW, -iH}, {0, 0, scaledWidth, scaledHeight}, {-topLeftX, -topLeftY}, 0, WHITE
      ); // Draw a part of a texture defined by a rectangle with 'pro' parameters

      RenderImGui();

      EndDrawing();
    }

    void startRaytracing() {
      ard.exit = false;

      for (auto &job : ard.pixelJobs->getJobsVector()) {
        job.color = vec3::Zero();
      }

      for (int t = 0; t < App::getNumThreads(); t++) {
        ard.threads.push_back(std::make_shared<std::thread>(Ray::Trace, ref(ard), &App::scene, t));
      }

      renderTime = 0;

      std::cout << "Raytracing started! (" << App::getNumThreads() << ")\n";
    }

    void BlitToBuffer() {
      static bool firstFrame = true;

      int w = App::getImageWidth();
      int h = App::getImageHeight();

      auto* pixelData = new vec3[w * h];
      auto jobs      = ard.pixelJobs->getJobsVector();

      // Copy over only the color data
      for (int i = 0; i < jobs.size(); ++i) {
        pixelData[i] = jobs[i].color;
      }

      if (firstFrame) {
        // Create texture from image containing the color data
        Image raytraced = {
            .data    = pixelData,
            .width   = w,
            .height  = h,
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

        if (App::saveOnRender)
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

            for (int t = 0; t < App::getNumThreads(); t++) {
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

      int w = App::getImageWidth(), h = App::getImageHeight();

      auto t  = std::time(nullptr);
      auto tm = *std::localtime(&t);
      ss << "screenshots/" << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "_" << w << "x" << h << "_"
         << App::rtSettings.samplesPerPixel << "_" << App::rtSettings.maxDepth << ".bmp";

      auto jobs = ard.pixelJobs->getJobsVector();

      // Tried writing using 32 bit (perc component) RGB and RGBA to no avail
      // The only way that this works is to convert each component to an 8 bit value
      // then write the converted data.
      u_char * pixels = new u_char[w * h * 3];
      for (int i = 0, j = 0; i < w * h * 3; i += 3, ++j) {
        Color c = jobs[j].color.toRaylibColor(255);
        pixels[i + 0] = c.r;
        pixels[i + 1] = c.g;
        pixels[i + 2] = c.b;
      }

      // Flip verticall for correct oreintation.
      stbi_flip_vertically_on_write(1);
      
      stbi_write_bmp(ss.str().c_str(), w, h, 3, pixels);

      delete[] pixels;

      std::cout << "Finished render: " << ss.str() << std::endl;
    }
  };
} // namespace rt