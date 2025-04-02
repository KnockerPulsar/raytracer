#include "app.h"

#include "AsyncRenderData.h"
#include "Constants.h"
#include "Defs.h"
#include "Ray.h"
#include "Scene.h"
#include "data_structures/JobQueue.h"
#include "editor/editor.h"
#include "raytracer.h"
#include "rt.h"

#include <raylib.h>
#include <rlImGui.h>

#include <fstream>
#include <memory>
#include <string>

namespace rt {
  void App::setup() {
    editor->setNextState(rt);
    rt->setNextState(editor);

    rlImGuiSetup(true);
    SetTargetFPS(60); // Not like we're gonna hit it...
    SetExitKey(KEY_NULL);

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard;
  }

  App::App(CliConfig config)
      : numThreads(config.numThreads), ard([&] {
          // AsyncRenderData tries to create a RenderTexture which requires a
          // window to be created
          InitWindow(config.editorWidth, config.editorHeight, rt::constants::title);
          return AsyncRenderData(config.imageWidth, config.imageHeight, config.editorWidth, config.editorHeight,
                                 config.numThreads);
        }()),
        editorWidth(config.editorWidth), editorHeight(config.editorHeight),
        scene(config.pathToScene.empty() ? Scene::Earth(config.imageWidth, config.imageHeight)
                                         : Scene::Load(config.imageWidth, config.imageHeight, config.pathToScene)),
        editor(std::make_shared<Editor>(this, config, scene)), rt(std::make_shared<Raytracer>(this, ard)),
        currentState(editor) {
    setup();
  }

  void App::run() {
    while (!WindowShouldClose() && !shouldQuit) {
      checkInput();
      currentState->onUpdate();
    }
  }

  void App::onFrameRender() const {}

  void App::checkInput() {
    int keyPressed = GetKeyPressed();

    // FIXME: After focusing an imgui widget, switching between the editor and
    // raytracer state automatically selects an imgui widget
    if (ImGui::GetIO().WantCaptureKeyboard && ImGui::GetIO().NavVisible && ImGui::GetIO().NavActive)
      return;

    switch (keyPressed) {
    case KEY_E: {
      currentState = currentState->toNextState();
      break;
    }
    case KEY_Q: {
      if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
        quit();
      break;
    }
    }
  }

  void App::changeScene(Scene scene) {
    this->scene = scene;

    editor->changeScene(&this->scene);
    rt->changeScene(&this->scene);
  }

  void App::quit()
  {
    shouldQuit = true;
  }

  App::~App() {
    // Note that if you kill the application in fullscreen, the resolution won't
    // reset to native.

    UnloadRenderTexture(ard.raytraceRT);

    rlImGuiShutdown();
    CloseWindow();
  }

  // Quick way of exporting hardcoded scenes into JSON
  void App::jsonExportTest() {

    rt::Scene     scene = rt::Scene::CornellBox(600, 600);
    json          json  = scene.toJson();
    std::ofstream output("scenes/cornell.json");
    output << std::setw(4) << json << '\n';
    output.close();
  }
} // namespace rt
