#include "Scene.h"
#include "app.h"
/*
 TODO:
    Finish up scene (de)serialization

    Hittable material from the editor (change material from editor).
      - View done
      - Logic left

    Finish up ImGuizmo integration. Swapping between translate and rotate. Investigate twitchy rotation issue.
    Regenerate BVH tree automatically on object transform with ImGuizmo

    Modify all hittables so that they're centered at the world origin to use scaling matrices.

    Cleanup call heirarchy. i.e. Each call to boundingBox currently requires us to call
    transformation.regenAABB() in case the object was moved. Perhaps something akin to
    IRasterizable.RasterizeTransformed()?

    Cache AABB instead of calculating it every iteration

    Re-implement incremental rendering with job queue chunks

    Configure clangd to format in a better way
    Clean up code and naming
*/

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

int main() {
  rt::App app(1200, 600, "scenes/cornell.json");
  app.run();

  // rt::App::jsonExportTest();

  return 0;
}
