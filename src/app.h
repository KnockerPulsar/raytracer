#pragma once
#include "AsyncRenderData.h"
#include "Scene.h"
#include <string>

namespace rt {
  class Editor;
  class Raytracer;
  class IState;

  class App {
    inline static AsyncRenderData ard;

    inline static sPtr<IState>    currentState;
    inline static sPtr<Editor>    editor;
    inline static sPtr<Raytracer> rt;

    inline static bool fullscreen = false;
    inline static App* app;
  public:
    inline static Scene           scene;
    inline static int numThreads, editorWidth, editorHeight;

    bool saveOnRender = true;
    void setup(int numThreads);

    App(int imageWidth, int imageHeight, int editorWidth, int editorHeight, std::string pathToScene, int numThreads);
    App(int imageWidth, int imageHeight, int editorWidth, int editorHeight, Scene scene, int numThreads);

    void run();
    void onFrameRender();
    void checkInput();

    static Scene& changeScene(std::string pathToScene);
    static Scene& changeScene(Scene scene);

    ~App();

    // Quick way of exporting hardcoded scenes into JSON
    static void jsonExportTest();
    static void jsonImportTest();

    static Scene           *getScene() { return &scene; }
    static AsyncRenderData *getARD() { return &ard; }
    static int              getNumThreads() { return numThreads; }

    static int getImageWidth()  { return rt::App::scene.imageWidth; };
    static int getImageHeight()  { return rt::App::scene.imageHeight; };

    static int getEditorWidth()  { return rt::App::editorWidth; };
    static int getEditorHeight()  { return rt::App::editorHeight; };

    static float getAspectRatio() { return float(getImageWidth()) / getImageHeight(); }

    static App &getApp() { return *App::app; };
    static void setApp(App &app) { App::app = &app; }

    static void changeNumThreads(int newNumThreads) {
      numThreads = newNumThreads;
      ard.changeNumThreads(numThreads);
    }
  };
} // namespace rt