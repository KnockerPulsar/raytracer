#include "app.h"
#include "AsyncRenderData.h"
#include "Camera.h"
#include "Defs.h"
#include "Hittable.h"
#include "Ray.h"
#include "RenderAsync.h"
#include "Scene.h"
#include "Transformation.h"
#include "data_structures/JobQueue.h"
#include "data_structures/Pixel.h"
#include "editor/editor.h"
#include "raytracer.h"
#include "rlImGui.h"
#include "rt.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <string>
#include <vector>

namespace rt {
  void App::setup(int imageWidth, int imageHeight, int numThreads) {
    ard = AsyncRenderData(numThreads);

    editor = std::make_shared<Editor>(ard);

    rt = std::make_shared<Raytracer>(ard);

    editor->nextState = rt;
    rt->nextState     = editor;

    editor->app = this;
    rt->app     = this;

    currentState = editor;

    rlImGuiSetup(true);
    SetTargetFPS(60); // Not like we're gonna hit it...

    if (fullscreen)
      ToggleFullscreen();

    ImGuiIO &io = ImGui::GetIO();
  }

  App::App(int imageWidth, int imageHeight, int editorWidth, int editorHeight, std::string pathToScene, int numThreads)
      : App(imageWidth,
            imageHeight,
            editorWidth,
            editorHeight,
            Scene::Load(imageWidth, imageHeight, pathToScene),
            numThreads) {}

  App::App(int imageWidth, int imageHeight, int editorWidth, int editorHeight, Scene scene, int numThreads)
      : numThreads(numThreads), editorWidth(editorWidth), editorHeight(editorHeight), imageHeight(imageHeight),
        imageWidth(imageWidth) {

    InitWindow(editorWidth, editorHeight, title.c_str());
    App::setApp(*this);
    setup(imageWidth, imageHeight, numThreads);
    changeScene(scene);
  }

  void App::run() {
    while (!WindowShouldClose()) {
      checkInput();
      currentState->onUpdate();
    }
  }

  void App::onFrameRender() {
    auto objects = scene.worldRoot->getChildrenAsList();
  }

  void App::checkInput() {
    int keyPressed = GetKeyPressed();

    if (ImGui::GetIO().WantCaptureKeyboard)
      return;

    switch (keyPressed) {

    case KEY_F: {
      fullscreen = !fullscreen;
      ToggleFullscreen();
      break;
    }

    case KEY_SPACE: {
      rt->showProg = !rt->showProg;
      break;
    }

    case KEY_E: {
      currentState = currentState->toNextState();
      break;
    }
    }
  }
  void App::changeScene(Scene scene) {
    this->scene = scene;

    editor->changeScene(&this->scene);
    rt->changeScene(&this->scene);
  }

  void App::changeScene(std::string pathToScene) { changeScene(Scene::Load(imageWidth, imageHeight, pathToScene)); }

  App::~App() {
    // Note that if you kill the application in fullscreen, the resolution won't
    // reset to native.

    if (fullscreen)
      ToggleFullscreen();

    rlImGuiShutdown();

    UnloadRenderTexture(ard.rasterRT);
    UnloadRenderTexture(ard.raytraceRT);

    rlImGuiShutdown();
    CloseWindow();
  }

  // Quick way of exporting hardcoded scenes into JSON
  void App::jsonExportTest() {

    rt::Scene     scene = rt::Scene::CornellBox(600, 600);
    json          json  = scene.toJson();
    std::ofstream output("scenes/cornell.json");
    output << std::setw(4) << json << std::endl;
    output.close();
  }
} // namespace rt
