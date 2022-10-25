#pragma once
#include "data_structures/vec3.h"
#include <thread>

namespace Defaults {
  #ifdef DEBUG
  const int imageWidth   = 100;
  inline static int availableThreads = 1;
  #else
  const int imageWidth   = 512;
  inline static int availableThreads = std::thread::hardware_concurrency() - 2;
  #endif

  const int editorWidth  = 1920;
  const int editorHeight = 1080;

  const int samplesPerPixel = 32;
  const int maxDepth = 4;

  const vec3 backgroundColor = vec3::Zero();

  const int ballGridWidth = 11;

  const std::string scenePath = "default";
} // namespace Defaults