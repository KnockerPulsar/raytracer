#pragma once
#include "AsyncRenderData.h"
#include "Scene.h"
#include <string>

namespace rt {
  class Editor;
  class Raytracer;
  class IState;

  class App {

    Scene           scene;
    AsyncRenderData ard;

    sPtr<Editor>    editor;
    sPtr<Raytracer> rt;

    int imageWidth, imageHeight, numThreads;

    bool fullscreen = false;

    sPtr<IState> currentState;

  public:
    bool saveOnRender = true;
    void setup(int imageWidth, int imageHeight, int numThreads);

    App(int imageWidth, int imageHeight, int numThreads);
    App(int imageWidth, int imageHeight, std::string pathToScene, int numThreads);
    App(int imageWidth, int imageHeight, Scene scene, int numThreads);

    void run();
    void onFrameRender();
    void checkInput();
    void changeScene(std::string pathToScene);
    void changeScene(Scene scene);

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