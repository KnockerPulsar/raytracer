#include "app.h"
#include "../vendor/rlImGui/rlImGui.h"
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
#include <ostream>
#include <raylib.h>
#include <vector>

namespace rt {
  App::App() {
    // Rendering constants for easy modifications.
    // Only used when creating hardcoded scenes
    const int   imageWidth      = 1200;
    const float aspectRatio     = 16 / 9.0;
    bool        fullscreen      = false;
    bool        showProg        = false;
    int         incRender       = 0;
    bool        raster          = true;
    bool        allFinished     = true;
    bool        rtInit          = false;

    InitWindow(imageWidth, imageWidth / aspectRatio, title.c_str());

    ard = RenderAsync::Perpare(imageWidth, aspectRatio)
              .setScene(std::make_shared<Scene>(
                  Scene::CornellBox(imageWidth, imageWidth / aspectRatio)));

    ard.rasterRT   = LoadRenderTexture(imageWidth, imageWidth / aspectRatio);
    ard.raytraceRT = LoadRenderTexture(imageWidth, imageWidth / aspectRatio);

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
  int App::jsonExportTest() {
    rt::Scene scene = rt::Scene::Load("cornell.json");
    // rt::Scene     scene = rt::Scene::CornellBox(600, 600, 50, 100);
    json          json = scene;
    std::ofstream output("cornell.json");
    output << std::setw(4) << json << std::endl;
    output.close();

    return 0;
  }
} // namespace rt