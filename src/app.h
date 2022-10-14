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

    int imageWidth, imageHeight, numThreads, editorWidth, editorHeight;

    bool fullscreen = false;

    sPtr<IState> currentState;
    inline static App* app;

  public:
    bool saveOnRender = true;
    void setup(int imageWidth, int imageHeight, int numThreads);

    App(
        int imageWidth, int imageHeight, int editorWidth, int editorHeight, std::string pathToScene, int numThreads
    );
    App(int imageWidth, int imageHeight, int editorWidth, int editorHeight, Scene scene, int numThreads);

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

    int getImageWidth() const { return imageWidth; };
    int getImageHeight() const { return imageHeight; };

    int getEditorWidth() const { return editorWidth; };
    int getEditorHeight() const { return editorHeight; };

    static App &getApp() { return *App::app; };
    static void setApp(App &app) { App::app = &app; }

    void changeNumThreads(int newNumThreads) {
      numThreads = newNumThreads;
      ard.changeNumThreads(numThreads);
    }
  };
} // namespace rt