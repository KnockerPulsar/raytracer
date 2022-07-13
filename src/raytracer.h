#include "AsyncRenderData.h"
#include "Constants.h"
#include "IState.h"
#include "Ray.h"
#include "RenderAsync.h"
#include "stb_image_write.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <raylib.h>
#include <rlImGui.h>
#include <sstream>

using std::ref;

namespace rt {
  class Raytracer : public IState {
    bool allFinished;

  public:
    bool             showProg = true;
    AsyncRenderData &ard;

    Raytracer(AsyncRenderData &ard, int imageWidth, int imageHeight) : ard(ard) {
      ard = AsyncRenderData(imageWidth, imageHeight);
    }

    virtual void onEnter() { startRaytracing(); }

    virtual void onExit() {
      ard.KillThreads();

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

      allFinished = false;
    }

    virtual void onUpdate() {
      BeginDrawing();

      onFinished();
      RenderImGui();

      EndDrawing();
    }

    void startRaytracing() {
      ard.exit = false;

      for (auto &job : ard.pixelJobs->getJobsVector()) {
        job.color = vec3::Zero();
      }

      for (int t = 0; t < NUM_THREADS; t++) {
        ard.threads.push_back(std::make_shared<std::thread>(Ray::Trace, ref(ard), getScene() , t));
      }
    }

    // TODO: Figure out a cleaner way
    void BlitToBuffer() {
      BeginTextureMode(ard.raytraceRT);

      auto pixelJobsVector = ard.pixelJobs->getJobsVector();
      for (int i = 0; i < ard.pixelJobs->numberOfJobs; i++) {
        Pixel &pixel = pixelJobsVector[i];

        // Clamp r, g, and b to prevent underflows and artifacts

        Color clr = ColorFromFloats(Clamp(pixel.color.x, 0, 1), Clamp(pixel.color.y, 0, 1), Clamp(pixel.color.z, 0, 1));

        // pixel.color = vec3::Zero();

        DrawPixel(pixel.x, pixel.y, clr);
      }

      EndTextureMode();
    }

    bool onFinished() {
      ClearBackground(BLACK);

      if (allFinished = ard.pixelJobs->jobsDone(); allFinished) {
        ard.KillThreads();
        
        BlitToBuffer();

        if(app->saveOnRender)
          Autosave();
      }

      DrawTextureRec(
          ard.raytraceRT.texture,
          (Rectangle){0, 0, (float)getScene()->imageWidth, (float)getScene()->imageHeight},
          (Vector2){0, 0},
          WHITE
      );

      // printf("%s\n", allFinished ? "all done" : "not yet");
      return allFinished;
    }

    void RenderImGui() {

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
    }
  };
} // namespace rt