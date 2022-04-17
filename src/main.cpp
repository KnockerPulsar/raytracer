#include "../vendor/imgui/imgui.h"
#include "../vendor/rlImGui/rlImGui.h"
#include "AsyncRenderData.h"
#include "Ray.h"
#include "RenderAsync.h"
#include "Scene.h"
#include "Transformation.h"
#include <algorithm>
#include <fstream>
#include <iostream>

/*
 TODO: finish json reading/writing
    Finish transform serialization/deserialization
    Add a way to compose/update a transformation
    Configure clangd to format in a better way
    Clean up code a naming
*/

/*
  TODO: Figure out to streamline planes into only one class
    Perhaps we can use the base XZ plane and rotate it to create YZ and XZ
  (perhaps also arbitrary planes?)
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
int main() {
  rt::Scene     scene = rt::Scene::Load("cornell.json");
  // rt::Scene     scene = rt::Scene::CornellBox(600, 600, 50, 100);
  json          json  = scene;
  std::ofstream output("cornell.json");
  output << std::setw(4) << json << std::endl;
  output.close();

  return 0;
}

int main1() {
  // Rendering constants for easy modifications.
  // Only used when creating hardcoded scenes
  const int   imageWidth      = 600;
  const float aspectRatio     = 16 / 9.0;
  const int   samplesPerPixel = 20;
  const int   maxDepth        = 10;
  bool        fullscreen      = false;
  bool        showProg        = false;
  int         incRender       = 1;

  SceneID sceneID = SceneID::cornell;

  AsyncRenderData asyncRenderData = RenderAsync::Perpare(
      imageWidth, aspectRatio, maxDepth, samplesPerPixel, sceneID, incRender);

  // asyncRenderData.currScene = rt::Scene::Load("cornell.json");
  asyncRenderData.currScene = rt::Scene::CornellBox(
      imageWidth, imageWidth / aspectRatio, maxDepth, samplesPerPixel);

  if (fullscreen)
    ToggleFullscreen();

  bool allFinished = true;
  while (!WindowShouldClose()) {

    // Update camera and start async again if last frame is done
    if (allFinished) {
      RenderAsync::ResetThreads(asyncRenderData);

      // for (auto &&obj : asyncRenderData.currScene.objects.objects) {
      //   auto t = obj->transformation.translate;
      //   auto r = obj->transformation.rotate;
      //   r.z += 1.0f;
      //   obj->transformation = rt::Transformation(t,r);
      // }
    }

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
