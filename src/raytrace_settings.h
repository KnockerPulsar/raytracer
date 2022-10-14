#pragma once

#include "IImguiDrawable.h"
#include "rt.h"

namespace rt {
  struct RayTracingSettings : public IImguiDrawable {
    int samplesPerPixel = Defaults::samplesPerPixel, maxDepth = Defaults::maxDepth, imageWidth = Defaults::imageWidth,
        imageHeight = Defaults::imageWidth, numThreads;

    RayTracingSettings &setSamplesPerPixel(int newSpp);
    RayTracingSettings &setMaxDepth(int newMd);
    RayTracingSettings &setImageWidth(int newIW);
    RayTracingSettings &setImageHeight(int newIH);

    void OnBaseImgui() override;
  };
} // namespace rt