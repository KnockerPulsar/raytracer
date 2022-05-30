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
#include "rt.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>
#include <raylib.h>
#include <vector>

/*
 TODO:
    Finish up basic shape rasterization for hittables
    
    Finish up ImGuizmo integration 

    Figure out a way to display different materials in the editor and modify them. Perhaps
    move materials to the base hittable to avoid duplication?

    Use a condition variable start stop threads on editor<->raytracer transition

    Cleanup call heirarchy. i.e. Each call to boundingBox currently requires us to call
    transformation.regenAABB() in case the object was moved. Perhaps something akin to
    IRasterizable.RasterizeTransformed()?

    Move rendering settings to ImGui (samples per pixel, max depth)

    Cache AABB instead of calculating it every iteration

    Configure clangd to format in a better way
    Clean up code and naming
*/

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

// Scenes
// scene1,
// scene2,
// random,
// random_moving,
// two_spheres,
// earth,
// light,
// cornell

using rt::RenderAsync, rt::AsyncRenderData, rt::SceneID;

// Quick way of exporting hardcoded scenes into JSON
int main1() {
  rt::Scene scene = rt::Scene::Load("cornell.json");
  // rt::Scene     scene = rt::Scene::CornellBox(600, 600, 50, 100);
  json          json = scene;
  std::ofstream output("cornell.json");
  output << std::setw(4) << json << std::endl;
  output.close();

  return 0;
}

int main() {
  // Rendering constants for easy modifications.
  // Only used when creating hardcoded scenes
  const int   imageWidth      = 1200;
  const float aspectRatio     = 16 / 9.0;
  const int   samplesPerPixel = 100;
  const int   maxDepth        = 100;
  bool        fullscreen      = false;
  bool        showProg        = false;
  int         incRender       = 0;
  bool        raster          = true;
  bool        allFinished     = true;
  bool        rtInit          = false;

  SceneID sceneID = SceneID::cornell;

  AsyncRenderData asyncRenderData =
      RenderAsync::Perpare(imageWidth, aspectRatio, maxDepth, samplesPerPixel, sceneID, incRender);

  // asyncRenderData.currScene = rt::Scene::Load("cornell.json");
  asyncRenderData.currScene =
      rt::Scene::CornellBox(imageWidth, imageWidth / aspectRatio, maxDepth, samplesPerPixel);

  rt::Camera &cam    = asyncRenderData.currScene.cam;
  rt::Editor *editor = new rt::Editor(sPtr<rt::Scene>(&asyncRenderData.currScene), &cam);
  editor->screenRT = asyncRenderData.rasterRT;

  if (fullscreen)
    ToggleFullscreen();

  auto onFrameRender = [&] {
    for (auto &&obj : asyncRenderData.currScene.worldRoot->getChildrenAsList()) {
      auto t = obj->transformation.translate;
      auto r = obj->transformation.rotate;
      r.y += 1.0f;
      obj->setTransformation(t, r);
    }
  };

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  

  auto checkInput = [&] {
    int keyPressed = GetKeyPressed();
    switch (keyPressed) {
    case KEY_F: {
      fullscreen = !fullscreen;
      ToggleFullscreen();
      break;
    }
    case KEY_SPACE: {
      showProg = !showProg;
      break;
    }
    case KEY_E: {
      raster = !raster;
      break;
    }
    }
  };

  while (!WindowShouldClose()) {
    checkInput();

    if (!raster) {

      if (!rtInit) {
        RenderAsync::Start(asyncRenderData);
        rtInit = true;
      }

      // Update camera and start async again if last frame is done
      if (allFinished) {
        RenderAsync::ResetThreads(asyncRenderData);

        // onFrameRender();
      }

      BeginDrawing();

      // Check on thread progress. Draw finished threads to buffer.
      allFinished = RenderAsync::RenderFinished(asyncRenderData);
      RenderAsync::RenderImGui(showProg, asyncRenderData);

      EndDrawing();
    } else {
      editor->Update();
    }
  }

  rlImGuiShutdown();
  if (rtInit) // Should have threads running
    RenderAsync::Shutdown(fullscreen, asyncRenderData);

  return 0;
}
