#include "../vendor/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "AsyncRenderData.h"
#include "RenderAsync.h"
#include "Scene.h"
#include <algorithm>
#include <fstream>
#include <iostream>

/*
 TODO: finish json reading/writing 
    Need some way to encode transforms 
    Currently, transforms (translation and rotation) are hittables that apply some transformation on the ray itself when they interact
    
    translation(object) -> AKA composition

    we somehow have to invert this structure on serialization...
    OR keep it how it is and work recursively   
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

int main1(){
  rt::Scene scene = rt::Scene::CornellBox(600, 600, 50, 100);
  json json = scene;
  std::ofstream output("cornell.json");
  output << std::setw(4) << json << std::endl;
  output.close();
}

int main() {
  // Rendering constants for easy modifications.
  const int   imageWidth      = 600;
  const float aspectRatio     = 1;
  const int   samplesPerPixel = 500;
  const int   maxDepth        = 100;
  bool        fullscreen      = false;
  bool        showProg        = false;
  int         incRender       = 1;

  SceneID sceneID = SceneID::cornell;

  AsyncRenderData asyncRenderData = RenderAsync::Perpare(
      imageWidth, aspectRatio, maxDepth, samplesPerPixel, sceneID, incRender);

  asyncRenderData.currScene = rt::Scene::Load("cornell.json");

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
