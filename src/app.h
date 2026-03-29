#pragma once
#include "common/AsyncRenderData.h"
#include "editor/scene.h"

#include <string>

struct CliConfig {
  glm::ivec2     imageSize;
  glm::ivec2     editorSize;
  int         numThreads = 6;
  std::string pathToScene;
  bool        potato_mode = false;
};

namespace rt {
  class Editor;
  class Raytracer;
  class IState;

  class App {

    editor::Scene           scene;
    AsyncRenderData ard;

    sPtr<Editor>    editor;
    sPtr<Raytracer> rt;

    int  numThreads;
    bool shouldQuit = false;

    sPtr<IState> currentState;

  public:
    glm::ivec2 const editorSize;

    bool saveOnRender = true;
    void setup();

    App(CliConfig);

    void run();
    void onFrameRender() const;
    void checkInput();
    void changeScene(Scene scene);
    void quit();

    ~App();

    // Quick way of exporting hardcoded scenes into JSON
    static void jsonExportTest();

    static void jsonImportTest();

    editor::Scene           *getScene() { return &scene; }
    AsyncRenderData *getARD() { return &ard; }
    int              getNumThreads() const { return numThreads; }

    void changeNumThreads(int newNumThreads) {
      numThreads = newNumThreads;
      ard.changeNumThreads(numThreads);
    }
  };
} // namespace rt
