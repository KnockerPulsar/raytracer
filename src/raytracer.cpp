#include "raytracer.h"

#include "IState.h"
#include "data_structures/Pixel.h"
#include "editor/Utils.h"

#include <imgui.h>

#include <iostream>
#include <raylib.h>

rt::Raytracer::Raytracer(App *const app, AsyncRenderData &ard) : IState(app), ard(ard) {}

void rt::Raytracer::onEnter() { startRaytracing(); }

void rt::Raytracer::onExit() {
  ard.KillThreads();

  // Reset job queue chunks
  ard.pixelJobs->setCurrentChunkStart(0);

  // Reset thread times and progress
  for (int i = 0; i < app->getNumThreads(); i++) {
    ard.threadTimes[i]     = 0;
    ard.threadProgress[i]  = 0;
    ard.finishedThreads[i] = false;
  }

  // Clear results from previous job.
  for (auto it = ard.pixelJobs->getJobsVector().begin(); it != ard.pixelJobs->getJobsVector().end(); ++it) {
    it->color = vec3(0);
  }

  allFinished = false;
}

void rt::Raytracer::onUpdate() {
  if(IsKeyPressed(KEY_SPACE))
    viewState.showProgress = !viewState.showProgress;

  BeginDrawing();

  onFinished();
  RenderImGui();

  EndDrawing();
}

void rt::Raytracer::startRaytracing() {
  ard.exit = false;

  for (auto &job : ard.pixelJobs->getJobsVector()) {
    job.color = vec3::Zero();
  }

  for (int t = 0; t < app->getNumThreads(); t++) {
    ard.threads.push_back(std::make_shared<std::thread>(Ray::Trace, std::ref(ard), getScene(), t));
  }
}

void rt::Raytracer::BlitToBuffer() {

  auto *pixelData = new Color[getScene()->imageWidth * getScene()->imageHeight];
  auto  jobs      = ard.pixelJobs->getJobsVector();

  // Copy over only the color data
  for (int i = 0; i < jobs.size(); ++i) {
    pixelData[i] = jobs[i].color.toRaylibColor(255);
  }

  // Unload old texture
  UnloadTexture(ard.raytraceRT.texture);

  // Create texture from image containing the color data
  Image raytraced = {.data    = pixelData,
                     .width   = getScene()->imageWidth,
                     .height  = getScene()->imageHeight,
                     .mipmaps = 1,
                     .format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

  // Deletes pixelData afte copying it to another buffer
  ImageFlipVertical(&raytraced);

  ard.raytraceRT.texture = LoadTextureFromImage(raytraced);

  UnloadImage(raytraced);
}

bool rt::Raytracer::onFinished() {
  ClearBackground(BLACK);

  bool finished = true;
  for (auto finishedThread : ard.finishedThreads) {
    finished &= finishedThread;
  }

  if (finished && !allFinished) {
    allFinished = true;

    ard.KillThreads();

    BlitToBuffer();

    if (app->saveOnRender)
      Autosave();
  }

  auto const fitSize = EditorUtils::FitIntoArea(ImVec2(app->editorWidth, app->editorHeight),
                                                ImVec2(ard.raytraceRT.texture.width, ard.raytraceRT.texture.height));

  float const dWidth  = app->editorWidth - fitSize.x;
  float const dHeight = app->editorHeight - fitSize.y;

  DrawTexturePro(ard.raytraceRT.texture,
                 (Rectangle){0, 0, (float)getScene()->imageWidth, (float)getScene()->imageHeight},
                 (Rectangle){dWidth / 2, dHeight / 2, fitSize.x, fitSize.y}, (Vector2){0, 0}, 0.0f, WHITE);
  return allFinished;
}

void rt::Raytracer::RenderImGui() {

  rlImGuiBegin();

  if (viewState.showProgress) {
    if (ImGui::Begin("Thread status", 0)) {
      ImGui::Text("%s", "Press space to toggle this menu, escape to quit");
      // ImGui::Checkbox("Incremental rendering", &ard.incRender);

      ImGui::Separator();

      ImGui::Text("Rendering progress");
      ImGui::SameLine();
      ImGui::ProgressBar(float(ard.pixelJobs->getCurrentChunkStart()) / ard.pixelJobs->getJobsVector().size());

      ImGui::Separator();

      ImGui::Checkbox("Show detailed thread progress", &viewState.detailedThreadProgress);

      if (viewState.detailedThreadProgress) {
        if (ImGui::BeginTable("Thread status", 3)) {
          ImGui::TableNextRow();

          for (int t = 0; t < app->getNumThreads(); t++) {
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
      }
    }
    ImGui::End();
  }

  rlImGuiEnd();
}

void rt::Raytracer::Autosave() {
  std::stringstream ss;

  auto t  = std::time(nullptr);
  auto tm = *std::localtime(&t);
  ss << "screenshots/" << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "_" << getScene()->imageWidth << "x"
     << getScene()->imageHeight << "_" << getScene()->settings.samplesPerPixel << "_" << getScene()->settings.maxDepth
     << ".bmp";

  std::cout << "Finished render: " << ss.str() << '\n';

  Image raytraced = LoadImageFromTexture(ard.raytraceRT.texture);

  // 4 components for RGBA;
  // Couldn't get raylibs ExportImage to work with BMP. It did work with PNG however
  stbi_write_bmp(ss.str().c_str(), raytraced.width, raytraced.height, 4, raytraced.data);

  UnloadImage(raytraced);
}

