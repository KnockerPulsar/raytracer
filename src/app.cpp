#include "app.h"

#include "AsyncRenderData.h"
#include "Defs.h"
#include "Hittable.h"
#include "Ray.h"
#include "RenderAsync.h"
#include "Scene.h"
#include "Transformation.h"
#include "data_structures/JobQueue.h"
#include "editor/editor.h"
#include "raytracer.h"
#include "rt.h"

#include <raylib.h>
#include <rlImGui.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

namespace rt {
  void App::setup() {
    editor->nextState = rt;
    rt->nextState     = editor;

    editor->app = this;
    rt->app     = this;

    rlImGuiSetup(true);
    SetTargetFPS(60); // Not like we're gonna hit it...

    if (fullscreen)
      ToggleFullscreen();

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }

  App::App(CliConfig config)
      : numThreads(config.numThreads), ard([&] {
          // AsyncRenderData tries to create a RenderTexture which requires a
          // window to be created
          InitWindow(config.editorWidth, config.editorHeight, title.c_str());
          return AsyncRenderData(config.imageWidth, config.imageHeight,
                                 config.editorWidth, config.editorHeight,
                                 config.numThreads);
        }()),
        editorWidth(config.editorWidth), editorHeight(config.editorHeight),
        scene(config.pathToScene.empty()
                  ? Scene::Earth(config.imageWidth, config.imageHeight)
                  : Scene::Load(config.imageWidth, config.imageHeight,
                                config.pathToScene)),
        editor(std::make_shared<Editor>(config, scene)),
        rt(std::make_shared<Raytracer>(ard)), currentState(editor) {
    setup();
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
    for (auto &&obj : objects) {
      auto t = obj->transformation.translate;
      auto r = obj->transformation.rotate;
      r.y += 1.0f;
      obj->transformation = Transformation(t, r);
    }
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

  App::~App() {
    // Note that if you kill the application in fullscreen, the resolution won't
    // reset to native.

    if (fullscreen)
      ToggleFullscreen();

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
