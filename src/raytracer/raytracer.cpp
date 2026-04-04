#include "raytracer.h"
#include "common/Hittable.h"

#include "common/Scene.h"
#include "common/IState.h"
#include "common/Util.h"
#include "materials//Material.h"

#include "data_structures/Pixel.h"

#include "editor/Utils.h"

#include <imgui.h>
#include <raylib.h>

#include <iostream>
#include <algorithm>

namespace {
  auto const samplesPerPixel = 100; // TODO
  auto const maxDepth        = 10; // TODO
}

rt::raytracer::Raytracer::Raytracer(App *const app, AsyncRenderData &ard, std::shared_ptr<Scene> const &compiledScene)
    : IState(app), ard(ard), compiledScene(compiledScene) {}

void rt::raytracer::Raytracer::onEnter() { startRaytracing(); }

void rt::raytracer::Raytracer::onExit() {
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

void rt::raytracer::Raytracer::onUpdate() {
  if(IsKeyPressed(KEY_SPACE))
    viewState.showProgress = !viewState.showProgress;

  BeginDrawing();

  onFinished();
  RenderImGui();

  EndDrawing();
}

void rt::raytracer::Raytracer::startRaytracing() {
  ard.exit = false;

  for (auto &job : ard.pixelJobs->getJobsVector()) {
    job.color = vec3::Zero();
  }

  for (int t = 0; t < app->getNumThreads(); t++) {
    ard.threads.push_back(std::make_shared<std::thread>(TraceRay, std::ref(ard), compiledScene, t));
  }
}

void rt::raytracer::Raytracer::BlitToBuffer() {

  auto const imageSize = compiledScene->imageSize();
  auto *pixelData = new Color[static_cast<int>(imageSize.x * imageSize.y)];
  auto  jobs      = ard.pixelJobs->getJobsVector();

  // Copy over only the color data
  for (int i = 0; i < jobs.size(); ++i) {
    pixelData[i] = jobs[i].color.toRaylibColor(255);
  }

  // Unload old texture
  UnloadTexture(ard.raytraceRT.texture);

  // Create texture from image containing the color data
  Image raytraced = {.data    = pixelData,
                     .width   = imageSize.x,
                     .height  = imageSize.y,
                     .mipmaps = 1,
                     .format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};

  // Deletes pixelData afte copying it to another buffer
  ImageFlipVertical(&raytraced);

  ard.raytraceRT.texture = LoadTextureFromImage(raytraced);

  UnloadImage(raytraced);
}

bool rt::raytracer::Raytracer::onFinished() {
  ClearBackground(BLACK);

  bool const finished = std::ranges::all_of(ard.finishedThreads, std::identity{});
  if (finished && !allFinished) {
    allFinished = true;

    ard.KillThreads();

    BlitToBuffer();

    if (app->saveOnRender)
      Autosave();
  }

  // TODO: figure out how to enable imgui's operator overloading
  auto const editorSize = ImVec2(app->editorSize.x, app->editorSize.y);
  auto const fitSize = EditorUtils::FitIntoArea(editorSize,
                                                ImVec2(ard.raytraceRT.texture.width, ard.raytraceRT.texture.height));

  float const dWidth  = editorSize.x - fitSize.x;
  float const dHeight = editorSize.y - fitSize.y;

  auto const imageSize = compiledScene->imageSize();
  DrawTexturePro(ard.raytraceRT.texture,
                 (Rectangle){0, 0, static_cast<float>(imageSize.x), static_cast<float>(imageSize.y)},
                 (Rectangle){dWidth / 2, dHeight / 2, fitSize.x, fitSize.y}, (Vector2){0, 0}, 0.0f, WHITE);
  return allFinished;
}

void rt::raytracer::Raytracer::TraceRay(AsyncRenderData &ard, std::shared_ptr<Scene> const &compiledScene,
                                        int threadIndex) {

  while (true) {
    auto start                = std::chrono::high_resolution_clock::now();
    auto [jobsStart, jobsEnd] = ard.pixelJobs->getChunk(ard, threadIndex);

    for (auto currentJob = jobsStart; currentJob != jobsEnd; ++currentJob) {

#ifdef FAST_EXIT
      // Exit prematurely if signaled to
      if (ard.exit == true)
        return;
#endif

      Pixel &job = *currentJob;

      int x = job.x;
      int y = job.y;

      for (int s = 0; s < samplesPerPixel; s++) {
        auto const imageSize = compiledScene->imageSize();
        float   u   = (x + RandomFloat()) / (imageSize.x - 1);
        float   v   = (y + RandomFloat()) / (imageSize.y - 1);
        rt::Ray ray = compiledScene->getRay(u, v);
        job.color += RayColor(ray, compiledScene, maxDepth);
      }

#ifdef GAMMA_CORRECTION
      // Gamma correction
      float r = job.color.x, g = job.color.y, b = job.color.z;

      float scale = 1.0 / samplesPerPixel;
      r           = sqrt(scale * r);
      g           = sqrt(scale * g);
      b           = sqrt(scale * b);

      job.color = vec3(r, g, b);
#else
      job.color /= float(samplesPerPixel);
#endif
      ard.threadProgress[threadIndex] = ((float)(currentJob - jobsStart) / (jobsEnd - jobsStart)) * 100;
    }
    auto stop      = std::chrono::high_resolution_clock::now();
    auto batchTime = duration_cast<std::chrono::milliseconds>(stop - start).count();
    ard.threadTimes[threadIndex] += batchTime;
  }
}

vec3 rt::raytracer::Raytracer::RayColor(const rt::Ray &r, std::shared_ptr<Scene> const &compiledScene, int depth) {
  // Limit max recursion depth
  if (depth <= 0) {
    return vec3::Zero();
  }

  rt::HitRecord rec;
  if (!compiledScene->hit(r, 0.001f, rt::constants::infinity, rec)) {
    return compiledScene->backgroundColor(r.direction);
  }

  rt::Ray scattered;
  vec3    attenuation;
  vec3    emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  return emitted + attenuation * RayColor(scattered, compiledScene, depth - 1);
}

void rt::raytracer::Raytracer::RenderImGui() {

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

void rt::raytracer::Raytracer::Autosave() {
  std::stringstream ss;

  auto t  = std::time(nullptr);
  auto tm = *std::localtime(&t);
  auto const imageSize = compiledScene->imageSize();
  ss << "screenshots/" << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "_" << imageSize.x << "x"
     << imageSize.y << "_" << samplesPerPixel << "_" << maxDepth
     << ".bmp";

  std::cout << "Finished render: " << ss.str() << '\n';

  Image raytraced = LoadImageFromTexture(ard.raytraceRT.texture);

  // 4 components for RGBA;
  // Couldn't get raylibs ExportImage to work with BMP. It did work with PNG however
  stbi_write_bmp(ss.str().c_str(), raytraced.width, raytraced.height, 4, raytraced.data);

  UnloadImage(raytraced);
}

