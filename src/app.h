#pragma once

#include "common/Defs.h"
#include "glm/ext/vector_int2.hpp"

#include <string>
#include <thread>

struct CliConfig {
  glm::ivec2   imageSize{900, 900};
  glm::ivec2   windowSize{1600, 900};
  uint64_t     numThreads{std::thread::hardware_concurrency()};
  bool         potatoMode{false};
  bool         saveOnRender{true};

  std::string  pathToScene;
  std::string  initialScene{"Cornell"};

  uint64_t numSamples{100};
  uint64_t maxDepth{10};

  std::optional<uint64_t> seed;
};

namespace rt {
  namespace editor { class Editor; }
  namespace raytracer { class Raytracer; }
  class IState;


  class App {
    //  We don't intend to point the editor to new instances.
    sPtr<editor::Editor> editor;

    bool shouldQuit = false;

  public:
    App(CliConfig);
    ~App();

    void run();
    void checkInput();
    void quit();

    // Quick way of exporting hardcoded scenes into JSON
    static void jsonExportTest();
    static void jsonImportTest();
  };
} // namespace rt
