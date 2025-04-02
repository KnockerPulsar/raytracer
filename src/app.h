#pragma once
#include "AsyncRenderData.h"
#include "Scene.h"

#include <string>

struct CliConfig
{
  int         imageWidth  = -1;
  int         imageHeight = -1;
  int         editorWidth = 1280;
  int         editorHeight = 720;
  int         numThreads  = 6;
  std::string pathToScene;
};

namespace rt {
  class Editor;
  class Raytracer;
  class IState;

  class App {

    Scene           scene;
    AsyncRenderData ard;

    sPtr<Editor>    editor;
    sPtr<Raytracer> rt;

    int  numThreads;
    bool shouldQuit = false;

    sPtr<IState> currentState;

  public:
    int const editorWidth, editorHeight;

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

    Scene           *getScene() { return &scene; }
    AsyncRenderData *getARD() { return &ard; }
    int              getNumThreads() const { return numThreads; }

    void changeNumThreads(int newNumThreads) {
      numThreads = newNumThreads;
      ard.changeNumThreads(numThreads);
    }
  };
} // namespace rt
