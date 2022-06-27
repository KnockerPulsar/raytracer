#include "app.h"
#include "../vendor/rlImGui/rlImGui.h"
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
#include "rt.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <vector>

namespace rt {
  void App::setup(AsyncRenderData &ard, int imageWidth, int imageHeight) {
    ard.rasterRT   = LoadRenderTexture(imageWidth, imageHeight);
    ard.raytraceRT = LoadRenderTexture(imageWidth, imageHeight);

    editor           = std::make_shared<Editor>(ard.currScene);
    editor->screenRT = ard.rasterRT;

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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }

  App::App(int imageWidth, int imageHeight) {
    InitWindow(imageWidth, imageHeight, title.c_str());

    ard = RenderAsync::Perpare(imageWidth, imageHeight)
              .setScene(std::make_shared<Scene>(Scene::CornellBox(imageWidth, imageHeight)));

    setup(ard, imageWidth, imageHeight);
  }

  App::App(int imageWidth, int imageHeight, std::string pathToScene) {
    InitWindow(imageWidth, imageHeight, title.c_str());

    ard = RenderAsync::Perpare(imageWidth, imageHeight)
              .setScene(std::make_shared<Scene>(Scene::Load(imageWidth, imageHeight, pathToScene)));

    setup(ard, imageWidth, imageHeight);
  }

  App::App(int imageWidth, int imageHeight, Scene scene) {

    InitWindow(imageWidth, imageHeight, title.c_str());

    ard = RenderAsync::Perpare(imageWidth, imageHeight).setScene(std::make_shared<Scene>(scene));
    setup(ard, imageWidth, imageHeight);
  }

  void App::run() {
    while (!WindowShouldClose()) {
      checkInput();
      currentState->onUpdate();
    }
  }

  void App::onFrameRender() {
    for (auto &&obj : ard.currScene->worldRoot->getChildrenAsList()) {
      auto t = obj->transformation.translate;
      auto r = obj->transformation.rotate;
      r.y += 1.0f;
      obj->setTransformation(t, r);
    }
  }

  void App::checkInput() {
    int keyPressed = GetKeyPressed();
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