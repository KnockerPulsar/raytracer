#pragma once
#include "data_structures/vec3.h"

namespace Defaults {
  const int imageWidth   = 100;
  const int editorWidth  = 1920;
  const int editorHeight = 1080;

  const int samplesPerPixel = 32;
  const int maxDepth = 4;

  const vec3 backgroundColor = vec3::Zero();

  const int ballGridWidth = 11;

  const std::string scenePath = "default";
} // namespace Defaults