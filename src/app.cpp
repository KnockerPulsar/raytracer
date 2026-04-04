#include "app.h"

#include "editor/editor.h"
#include "editor/scene.h"

#include "common/Constants.h"
#include "common/Defs.h"
#include "data_structures/JobQueue.h"

#include "raytracer/raytracer.h"

#include <raylib.h>
#include <rlImGui.h>

#include <memory>
#include <string>

namespace rt {
  App::App(CliConfig config)
      : numThreads(config.numThreads), ard([&] {
          // AsyncRenderData tries to create a RenderTexture which requires a
          // window to be created
          InitWindow(config.editorSize.x, config.editorSize.y, rt::constants::title);
          return AsyncRenderData(config.imageSize, config.editorSize, config.numThreads);
        }()),
        editorSize(config.editorSize),
        editor(std::make_shared<editor::Editor>(
            this, config,
            std::make_unique<editor::Scene>(config.pathToScene.empty()
                                                ? editor::Scene::Earth(config.imageSize.x, config.imageSize.y)
                                                : editor::Scene::Load(config.imageSize, config.pathToScene)))),
        currentState(editor) {
    rlImGuiSetup(true);
    SetTargetFPS(60); // Not like we're gonna hit it...
    SetExitKey(KEY_NULL);

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard;
  }

  void App::run() {
    while (!WindowShouldClose() && !shouldQuit) {
      checkInput();
      currentState.lock()->onUpdate();
    }
  }

  void App::rebuildRaytracer(std::shared_ptr<raytracer::Raytracer> newState) { rt = newState; }

  void App::checkInput() {
    int keyPressed = GetKeyPressed();

    // FIXME: After focusing an imgui widget, switching between the editor and
    // raytracer state automatically selects an imgui widget
    if (ImGui::GetIO().WantCaptureKeyboard && ImGui::GetIO().NavVisible && ImGui::GetIO().NavActive)
      return;

    switch (keyPressed) {
    case KEY_E: {
      auto currentStateLocked = currentState.lock();
      currentStateLocked->onExit();
      auto const nextState = currentStateLocked == editor ? std::shared_ptr<IState>{rt} : std::shared_ptr<IState>{editor};
      nextState->onEnter();

      currentState = nextState;

      break;
    }
    case KEY_Q: {
      if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
        quit();
      break;
    }
    }
  }

  void App::quit()
  {
    shouldQuit = true;
  }

  App::~App() {
    // Note that if you kill the application in fullscreen, the resolution won't
    // reset to native.

    // Destroy the editor before we close the window so the raster texture can
    // be unloaded properly.
    editor.reset();
    UnloadRenderTexture(ard.raytraceRT);

    rlImGuiShutdown();
    CloseWindow();
  }

  // Quick way of exporting hardcoded scenes into JSON
  // void App::jsonExportTest() {
  //   rt::editor::Scene     scene = rt::editor::Scene::CornellBox(600, 600);
  //   json          json  = scene.toJson();
  //   std::ofstream output("scenes/cornell.json");
  //   output << std::setw(4) << json << '\n';
  //   output.close();
  // }
} // namespace rt
