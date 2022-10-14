#pragma once
#include "AsyncRenderData.h"
#include "Scene.h"
#include <string>

struct RayTracingSettings {
  int  samplesPerPixel = Defaults::samplesPerPixel, maxDepth = Defaults::maxDepth, imageWidth = Defaults::imageWidth,
      imageHeight = Defaults::imageWidth, numThreads;

  RayTracingSettings &setSamplesPerPixel(int newSpp) {
    samplesPerPixel = newSpp;
    return *this;
  }
  RayTracingSettings &setMaxDepth(int newMd) {
    maxDepth = newMd;
    return *this;
  }

  RayTracingSettings &setImageWidth(int newIW) {
    if (newIW == -1)
      imageWidth = Defaults::imageWidth;
    else
      imageWidth = newIW;

    return *this;
  }

  RayTracingSettings &setImageHeight(int newIH) {

    if (newIH == -1)
      imageHeight = Defaults::imageWidth;
    else
      imageHeight = newIH;

    return *this;
  }
};

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
    inline static Scene              scene;
    inline static RayTracingSettings rtSettings;
    inline static int                editorWidth, editorHeight;

    inline static bool saveOnRender = true;
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
    static int              getNumThreads() { return rtSettings.numThreads; }

    static int getImageWidth()  { return rtSettings.imageWidth; };
    static int getImageHeight()  { return rtSettings.imageHeight; };

    static int getEditorWidth()  { return editorWidth; };
    static int getEditorHeight()  { return editorHeight; };

    static float getAspectRatio() { return float(getImageWidth()) / getImageHeight(); }

    static App &getApp() { return *app; };
    static void setApp(App &app) { App::app = &app; }

    static void changeNumThreads(int newNumThreads) {
      rtSettings.numThreads = newNumThreads;
      ard.changeNumThreads(rtSettings.numThreads);
    }
  };
} // namespace rt