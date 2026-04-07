#pragma once
#include "common/Scene.h" // TODO move to `raytracer/scene.h`

#include "glm/ext/vector_int2.hpp"
#include <memory>

namespace rt::raytracer {
    class Scene;
    struct Config {
      std::unique_ptr<Scene> compiledScene;

      uint64_t const seed;
      uint64_t const numSamples;
      uint64_t const maxDepth;
      uint64_t const numThreads;

      glm::ivec2 const imageSize;
    };

}

