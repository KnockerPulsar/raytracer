#pragma once
#include "common/AsyncRenderData.h"

#include <string>

struct CliConfig {
  glm::ivec2  imageSize{900, 900};
  glm::ivec2  editorSize{1280, 720};
  unsigned int numThreads{std::thread::hardware_concurrency()};
  std::string pathToScene;
  bool        potato_mode = false;
};

namespace rt {
  namespace editor { class Editor; }
  namespace raytracer { class Raytracer; }
  class IState;


  class App {
    AsyncRenderData ard;

    //  We don't intend to point the editor to new instances.
    sPtr<editor::Editor> const editor;
    sPtr<raytracer::Raytracer> rt;

    int  numThreads;
    bool shouldQuit = false;

    sPtr<IState> currentState;

  public:
    App(CliConfig);
    ~App();

    void run();
    void rebuildRaytracer(std::shared_ptr<raytracer::Raytracer> newState);
    void checkInput();
    void quit();

    // Quick way of exporting hardcoded scenes into JSON
    static void jsonExportTest();
    static void jsonImportTest();

    AsyncRenderData *getARD() { return &ard; }
    int              getNumThreads() const { return numThreads; }

    void changeNumThreads(int newNumThreads) {
      numThreads = newNumThreads;
      ard.changeNumThreads(numThreads);
    }

    glm::ivec2 const editorSize;
    bool saveOnRender = true;
  };
} // namespace rt
