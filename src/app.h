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
    App();
    void run();
    void onFrameRender();
    void checkInput();

    ~App();

    // Quick way of exporting hardcoded scenes into JSON
    static int jsonExportTest();
  };
} // namespace rt