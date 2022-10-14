#include "raytrace_settings.h"
#include "app.h"
#include "imgui.h"

namespace rt {

  RayTracingSettings &RayTracingSettings::setSamplesPerPixel(int newSpp) {
    samplesPerPixel = newSpp;
    return *this;
  }

  RayTracingSettings &RayTracingSettings::setMaxDepth(int newMd) {
    maxDepth = newMd;
    return *this;
  }

  RayTracingSettings &RayTracingSettings::setImageWidth(int newIW) {
    if (newIW != imageWidth) {
      if (newIW == -1)
        imageWidth = Defaults::imageWidth;
      else
        imageWidth = newIW;
    }

    return *this;
  }

  RayTracingSettings &RayTracingSettings::setImageHeight(int newIH) {
    if (newIH != imageHeight) {
      if (newIH == -1)
        imageHeight = Defaults::imageWidth;
      else
        imageHeight = newIH;
    }

    return *this;
  }

  void RayTracingSettings::OnBaseImgui() {
    ImGui::Begin("Raytrace settings");

    ImGui::DragInt("Samples per pixel", &samplesPerPixel, 1, 1, 500);
    ImGui::DragInt("Maximum depth", &maxDepth, 1, 1, 100);

    if (ImGui::InputScalar("Number of threads", ImGuiDataType_U32, &numThreads)) {
      App::changeNumThreads(numThreads);
    }

    ImGui::SliderInt2("Raytracing resolution", &imageWidth, 100, 1920);
    ImGui::End();
  }
} // namespace rt