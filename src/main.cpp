#include "../vendor/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "AsyncRenderData.h"
#include "RenderAsync.h"
#include "Scene.h"
#include <algorithm>
#include <iostream>

// TODO: Separate checking on threads and blitting finished thread pixels.
// TODO: Implement a clearer pipeline (Init, loop{Check inputs, render}, clean
// up).
// TODO: Add progress bar to shutdown loop.

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

using raytracer::RenderAsync, raytracer::AsyncRenderData, raytracer::SceneID;

int main() {
  // Rendering constants for easy modifications.
  const int   imageWidth      = 600;
  const float aspectRatio     = 1;
  const int   samplesPerPixel = 100;
  const int   maxDepth        = 20;
  bool        fullscreen      = false;
  bool        showProg        = true;
  int         incRender       = 1;

  SceneID sceneID = SceneID::scene1;

  AsyncRenderData asyncRenderData = RenderAsync::Perpare(
      imageWidth, aspectRatio, maxDepth, samplesPerPixel, sceneID, incRender);

  if (fullscreen)
    ToggleFullscreen();

  bool allFinished = true;
  while (!WindowShouldClose()) {

    // Update camera and start async again if last frame is done
    if (allFinished)
      RenderAsync::ResetThreads(asyncRenderData);

    RenderAsync::CheckInput(fullscreen, showProg);

    BeginDrawing();

    // Check on thread progress. Draw finished threads to buffer.
    allFinished = RenderAsync::RenderFinished(asyncRenderData);

    RenderAsync::RenderImGui(showProg, asyncRenderData);

    EndDrawing();
  }

  RenderAsync::Shutdown(fullscreen, asyncRenderData);

  return 0;
}
