#include "Scene.h"
#include "app.h"
#include <argumentum/argparse.h>
#include <cstdlib>
#include <ostream>
#include <string>
#include <tuple>
/*
 TODO:

    Make the number of threads a controllable variable
        - Mostly just used in AsyncRenderData when spawning threads

    Try applying transformations before rendering instead of transforming each ray depending on the object we're
 checking collision with
      - For boxes, planes, triangles: Should be as easy as applying the transform to the vertices once and recalculating
 normals
      - For spheres and parameteric shapes: might have to do runtime transformations on normals for example.

    Finish up scene (de)serialization
      - Saving and loading done
      - Need more testing
      - Need to export existing scenes out and test them
      - Need to filter out common field saving and loading into the base Hittable class.
      - Hittables don't load their name

    Regenerate BVH tree automatically on object transform with ImGuizmo?
      - At least after letting go
      - Need a regeneration method instead of using addChild(nullptr)

    Modify all hittables so that they're centered at the world origin to use scaling matrices.
      - Box, plane, sphere all done
      - Not sure about the rest

    Add the ability to change material textures

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

std::tuple<int, int, std::string, int> setupArguments(int argc, char **argv) {
  const int imageWidthDefault = 900;
  const int numThreadsDefault = 6;

  int         imageWidth  = -1;
  int         imageHeight = -1;
  int         numThreads  = 6;
  std::string pathToScene;

  argument_parser parser = argument_parser{};
  auto            params = parser.params();

  parser.config().program(argv[0]).description("Raytracer");
  parser.add_argument(imageWidth, "--image_width")
      .maxargs(1)
      .metavar("UNSIGNED INT")
      .absent(imageWidthDefault)
      .help("Window and rendering resolution width in pixels");

  parser.add_argument(imageHeight, "--image_height")
      .maxargs(1)
      .metavar("UNSIGNED INT")
      .absent(-1)
      .help("Window and rendering resolution width in pixels");

  parser.add_argument(pathToScene, "--scene")
      .maxargs(1)
      .metavar("STRING PATH")
      .absent("default")
      .help("Path to scene json");

  parser.add_argument(numThreads, "--threads")
      .maxargs(1)
      .metavar("UNSIGNED INT")
      .absent(numThreadsDefault)
      .help("Number of threads to raytrace with")
      .action([&](auto &target, const std::string &value) {
        int parsedValue = std::atoi(value.c_str());

        if (parsedValue <= 0) {
          std::cout << "WARNING: Invalid number of threads entered (" << value << "), using default number of threads ("
                    << numThreadsDefault << ")" << std::endl;
                    
          target = numThreadsDefault;
        } else {
          target = parsedValue;
        }
      });

  if (!parser.parse_args(argc, argv, 1))
    std::exit(1);

  return std::make_tuple(imageWidth, imageHeight, pathToScene, numThreads);
}

int main(int argc, char **argv) {

  auto [imageWidth, imageHeight, pathToScene, numThreads] = setupArguments(argc, argv);

  // Image width is set but image height is not
  if (imageHeight == -1) {
    imageHeight = imageWidth;
  }

  if (pathToScene != "default") {
    rt::App app(imageWidth, imageHeight, pathToScene, numThreads);
    app.run();
  } else {
    rt::App app(imageWidth, imageHeight, rt::Scene::Scene1(imageWidth, imageHeight), numThreads);
    app.run();
  }

  return 0;
}
