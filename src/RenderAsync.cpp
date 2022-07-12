#include "RenderAsync.h"
#include "Constants.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <exception>
#include <future>
#include <iostream>
#include <memory>
#include <numeric>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "../vendor/rlImGui/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "AsyncRenderData.h"
#include "Defs.h"
#include "Ray.h"
#include "Scene.h"
#include "data_structures/Pixel.h"

using std::pair, std::future, std::vector, std::async;

using rt::Hittable, rt::Pixel;

using std::string, std::vector, std::chrono::high_resolution_clock, std::chrono::duration_cast,
    std::chrono::milliseconds, std::pair, std::future, std::future, std::async, std::ref, std::make_pair, std::launch,
    std::future, std::async, std::ref, std::make_pair, std::launch;

namespace rt {
  void RenderAsync::ResetThreads(AsyncRenderData &ard) { ard.pixelJobs->awakeAllWorkers(); }
} // namespace rt