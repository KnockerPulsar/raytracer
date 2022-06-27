#pragma once
#include "AsyncRenderData.h"

namespace rt {
  class Editor;
  class Raytracer;
  class IState;

  class App {
    AsyncRenderData ard;
    sPtr<Editor>    editor;
    sPtr<Raytracer> rt;

    bool fullscreen = false;

    sPtr<IState> currentState;

  public:
    void setup(AsyncRenderData &ard, int imageWidth, int imageHeight);

    App(int imageWidth, int imageHeight);
    App(int imageWidth, int imageHeight, std::string pathToScene);
    App(int imageWidth, int imageHeight, Scene scene);

    void run();
    void onFrameRender();
    void checkInput();

    ~App();

    // Quick way of exporting hardcoded scenes into JSON
    static void jsonExportTest();

    static void jsonImportTest();
  };
} // namespace rt