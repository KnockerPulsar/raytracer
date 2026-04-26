#include "app.h"

#include "editor/editor.h"

#include "common/Constants.h"
#include "common/Defs.h"

#include "raytracer/raytracer.h"

#include <raylib.h>
#include <rlImGui.h>

#include <memory>

namespace rt {
  App::App(CliConfig config) {

    InitWindow(config.windowSize.x, config.windowSize.y, rt::constants::title);

    // Editor MUST be initialized after the window is created
    editor       = std::make_shared<editor::Editor>(this, config);

    rlImGuiSetup(true);
    SetTargetFPS(60); // Not like we're gonna hit it...
    SetExitKey(KEY_NULL);

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard;
  }

  void App::run() {
    while (!WindowShouldClose() && !shouldQuit) {
      checkInput();
      editor->onUpdate();
    }
  }

  void App::checkInput() {
    int keyPressed = GetKeyPressed();

    // FIXME: After focusing an imgui widget, switching between the editor and
    // raytracer state automatically selects an imgui widget
    if (ImGui::GetIO().WantCaptureKeyboard && ImGui::GetIO().NavVisible && ImGui::GetIO().NavActive)
      return;

    switch (keyPressed) {
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

    // Destroy the editor and raytracer before we close the window so the
    // raster and raytrace textures can be unloaded properly.
    editor.reset();

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
