#include "raytracer.h"
#include "common/Hittable.h"

#include "common/Scene.h"
#include "common/Util.h"
#include "materials//Material.h"

#include "data_structures/Pixel.h"
#include "data_structures/JobQueue.h"

#include "editor/Utils.h"

#include <imgui.h>
#include <raylib.h>

rt::raytracer::Raytracer::Raytracer(Config &&config, std::function<void(Frame)> onRenderDone)
    : ard(std::move(config)), onRenderDone(onRenderDone) {}

void rt::raytracer::Raytracer::onUpdate() {
  if (ard.isRaytracing() && ard.finishedRaytracing() && !raytracingDone) {
    raytracingDone = true;
    ard.KillThreads();

    auto const imageSize = ard.config.imageSize;
    auto      *pixelData = new Color[static_cast<int>(imageSize.x * imageSize.y)];
    auto       jobs      = ard.pixelJobs.getJobsVector();

    // Copy over only the color data
    for (int i = 0; i < jobs.size(); ++i) {
      pixelData[i] = jobs[i].color.toRaylibColor(255);
    }

    // Create texture from image containing the color data
    Image raytraced = {
        .data    = pixelData,
        .width   = imageSize.x,
        .height  = imageSize.y,
        .mipmaps = 1,
        .format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };

    onRenderDone(Frame{LoadTextureFromImage(raytraced)});
  }
}

void rt::raytracer::Raytracer::startRaytracing() {
  ard.exit = false;

  for (auto &job : ard.pixelJobs.getJobsVector()) {
    job.color = vec3::Zero();
  }

  for (int t = 0; t < ard.config.numThreads; t++) {
    ard.threads.emplace_back(TraceRay, std::ref(ard), t);
  }
}

void rt::raytracer::Raytracer::stopRaytracing() {
  ard.KillThreads();

  // Reset job queue chunks
  ard.pixelJobs.setCurrentChunkStart(0);

  ard.finishedThreads = 0;
  // Reset thread times and progress
  for (int i = 0; i < ard.config.numThreads; i++) {
    ard.threadTimes[i]    = 0;
    ard.threadProgress[i] = 0;
  }

  // Clear results from previous job.
  for (auto it = ard.pixelJobs.getJobsVector().begin(); it != ard.pixelJobs.getJobsVector().end(); ++it) {
    it->color = vec3(0);
  }

  raytracingDone = false;
}

void rt::raytracer::Raytracer::TraceRay(AsyncRenderData &ard, int threadIndex) {

  while (true) {
    auto start                = std::chrono::high_resolution_clock::now();
    auto [jobsStart, jobsEnd] = ard.pixelJobs.getChunk(threadIndex);

    for (auto currentJob = jobsStart; currentJob != jobsEnd; ++currentJob) {

#ifdef FAST_EXIT
      // Exit prematurely if signaled to
      if (ard.exit == true)
        return;
#endif


      Pixel &job = *currentJob;

      int x = job.x;
      int y = job.y;
      SetSeed(job.seed);

      auto const& compiledScene = ard.config.compiledScene;
      auto const samplesPerPixel = ard.config.numSamples;
      for (int s = 0; s < samplesPerPixel; s++) {
        auto const imageSize = ard.config.imageSize;
        float   u   = (x + RandomFloat()) / (imageSize.x - 1);
        float   v   = (y + RandomFloat()) / (imageSize.y - 1);
        rt::Ray ray = compiledScene->getRay(u, v);
        job.color += RayColor(ray, compiledScene, ard.config.maxDepth);
      }

#ifdef GAMMA_CORRECTION
      // Gamma correction
      float r = job.color.x;
      float g = job.color.y;
      float b = job.color.z;

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

vec3 rt::raytracer::Raytracer::RayColor(const rt::Ray &r, std::unique_ptr<Scene> const &compiledScene, int depth) {
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
      ImGui::ProgressBar(float(ard.pixelJobs.getCurrentChunkStart()) / ard.pixelJobs.getJobsVector().size());

      ImGui::Separator();

      ImGui::Checkbox("Show detailed thread progress", &viewState.detailedThreadProgress);

      if (viewState.detailedThreadProgress) {
        if (ImGui::BeginTable("Thread status", 3)) {
          ImGui::TableNextRow();

          for (int t = 0; t < ard.config.numThreads; t++) {
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

float rt::raytracer::Raytracer::progress() const {
  return ard.progress();
}

bool rt::raytracer::Raytracer::isRaytracing() const {
  return ard.isRaytracing();
}
