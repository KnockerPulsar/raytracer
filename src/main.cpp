#include "../vendor/rlImGui/rlImGui.h"
#include "AsyncRenderData.h"
#include "Camera.h"
#include "Ray.h"
#include "RenderAsync.h"
#include "Scene.h"
#include "Transformation.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>
#include <raylib.h>
#include <vector>

/*
 TODO:
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
  const int   samplesPerPixel = 1;
  const int   maxDepth        = 10;
  bool        fullscreen      = false;
  bool        showProg        = false;
  int         incRender       = 1;
  bool        raster          = true;

  SceneID sceneID = SceneID::cornell;

  AsyncRenderData asyncRenderData =
      RenderAsync::Perpare(imageWidth, aspectRatio, maxDepth, samplesPerPixel, sceneID, incRender);

  // asyncRenderData.currScene = rt::Scene::Load("cornell.json");
  asyncRenderData.currScene =
      rt::Scene::TransformationTest(imageWidth, imageWidth / aspectRatio, maxDepth, samplesPerPixel);

  rt::Camera &cam = asyncRenderData.currScene.cam;

  if (fullscreen)
    ToggleFullscreen();

  auto onFrameRender = [&] {
    for (auto &&obj : asyncRenderData.currScene.world.objects) {
      auto t = obj->transformation.translate;
      auto r = obj->transformation.rotate;
      r.y += 1.0f;
      obj->setTransformation(t, r);
    }
  };

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

  bool allFinished = true;
  while (!WindowShouldClose()) {
    checkInput();
    if (!raster) {
      // Update camera and start async again if last frame is done
      if (allFinished) {
        RenderAsync::ResetThreads(asyncRenderData);

        onFrameRender();
      }

      BeginDrawing();

      // Check on thread progress. Draw finished threads to buffer.
      allFinished = RenderAsync::RenderFinished(asyncRenderData);
      RenderAsync::RenderImGui(showProg, asyncRenderData);

      EndDrawing();
    } else {
      BeginDrawing();
      ClearBackground(BLACK);

      if (cam.controlType == rt::CameraControlType::flyCam)
        cam.UpdateEditorCamera(GetFrameTime());

      // Trial-and-error'd my way through figuring out these
      bool dragging = ImGui::IsMouseDragging(0);
      bool hovering = ImGui::IsAnyItemHovered();

      // Using ImGui's function since it might work better with the GUI
      if (ImGui::IsMouseClicked(0) && !hovering && !dragging) {
        float screenW = GetScreenWidth();
        float screenH = GetScreenHeight();

        float mouseX = GetMouseX();
        float mouseY = GetMouseY();


        
        rt::Camera::selectedObject =
            cam.CastRay({mouseX, mouseY}, {screenW, screenH}, &asyncRenderData.currScene.world);
      }

      cam.Rasterize(asyncRenderData.currScene.world.objects);
      cam.RenderImgui(&asyncRenderData.currScene.objects);

      onFrameRender();
      EndDrawing();
    }
  }

  rlImGuiShutdown();
  if (!raster)
    RenderAsync::Shutdown(fullscreen, asyncRenderData);

  return 0;
}
