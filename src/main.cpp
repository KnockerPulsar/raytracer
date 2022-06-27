#include "../vendor/argumentum/include/argumentum/argparse.h"
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

using namespace argumentum;

int main(int argc, char **argv) {

  int imageWidthDefault = 900;

  int         imageWidth  = -1;
  int         imageHeight = -1;
  std::string pathToScene;

  auto parser = argument_parser{};
  auto params = parser.params();

  parser.config().program(argv[0]).description("Raytracer");
  parser.add_argument(imageWidth, "--image_width")
      .maxargs(1)
      .metavar("INT")
      .absent(imageWidthDefault)
      .help("Window and rendering resolution width in pixels");

  parser.add_argument(imageHeight, "--image_height")
      .maxargs(1)
      .metavar("INT")
      .absent(-1)
      .help("Window and rendering resolution width in pixels");

  parser.add_argument(pathToScene, "--scene")
      .maxargs(1)
      .metavar("STRING PATH")
      .absent("default")
      .help("Path to scene json");

  if (!parser.parse_args(argc, argv, 1))
    return 1;

  // Image width is set but image height is not
  if (imageHeight == -1) {
    imageHeight = imageWidth;
  }

  if (pathToScene != "default") {
    rt::App app(imageWidth, imageHeight, pathToScene);
    app.run();
  } else {
    rt::App app(imageWidth, imageHeight, rt::Scene::Default(imageWidth, imageHeight));
    app.run();
  }

  // rt::App::jsonExportTest();

  return 0;
}
