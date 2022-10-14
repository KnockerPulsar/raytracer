#include "Scene.h"
#include "app.h"
#include "rt.h"
#include <argumentum/argparse.h>
#include <cstdlib>
#include <ostream>
#include <string>
#include <thread>
#include <tuple>
/*
 TODO:
    Add the ability to change material textures

    Can add a prepass before raytracing where triangles and AABBs  are transformed  
        Try applying transformations before rendering instead of transforming each ray depending on the object we're checking collision with
          - For boxes, planes, triangles: Should be as easy as applying the transform to the vertices once and recalculating normals
          - For spheres and parameteric shapes: might have to do runtime transformations on normals for example.

        Cache AABB instead of calculating it every iteration

        Modify all hittables so that they're centered at the world origin to use scaling matrices.
          - Box, plane, sphere all done
          - Not sure about the rest

    Finish up scene (de)serialization
      - Saving and loading done
      - Need more testing
      - Need to export existing scenes out and test them
      - Need to filter out common field saving and loading into the base Hittable class.
      - Hittables don't load their name

    Regenerate BVH tree automatically on object transform with ImGuizmo?
      - At least after letting go
      - Need a regeneration method instead of using addChild(nullptr)


    For previewing procedural textures, could sample a small (50x50) preview from the texture
      - Compute shader might be able to make things faster at the cost of more complexity
      - Caching results is a must, can't calculate a preview each frame (I think)
      - Need to setup a way to rebuild previews on parameter change -> Done manually
      - Perlin noise is difficult to preview since it's 3D 
      - Can have an option to have a second raytraced preview for a more precise preview.

    Cleanup call heirarchy. i.e. Each call to boundingBox currently requires us to call
    transformation.regenAABB() in case the object was moved. Perhaps something akin to
    IRasterizable.RasterizeTransformed()?

    Re-implement incremental rendering with job queue chunks
      - Would require blitting whenever a thread finishes a chunk
      - Might require a lock for proper operation

    Configure clangd to format in a better way

    Clean up code and naming 
      - Method naming convention jumps between PascalCase and camelCase
*/

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

using namespace argumentum;

std::tuple<int, int, int, int, std::string, int> setupArguments(int argc, char **argv) {

  int         imageWidth  = -1;
  int         imageHeight = -1;
  int         editorWidth  = -1;
  int         editorHeight = -1;

  int         numThreads;
  std::string pathToScene;

  argument_parser parser = argument_parser{};
  auto            params = parser.params();

  parser.config().program(argv[0]).description("Raytracer");
  params.add_parameter(imageWidth, "--image_width")
      .metavar("UNSIGNED INT")
      .absent(-1)
      .help("Window and rendering resolution width in pixels");

  params.add_parameter(imageHeight, "--image_height")
      .metavar("UNSIGNED INT")
      .absent(-1)
      .help("Window and rendering resolution width in pixels");

  params.add_parameter(editorWidth, "--editor_width").absent(Defaults::editorWidth);
  params.add_parameter(editorHeight, "--editor_height").absent(Defaults::editorHeight);

  params.add_parameter(pathToScene, "--scene")
      .metavar("STRING PATH")
      .absent(Defaults::scenePath)
      .help("Path to scene json");

  
  int availableThreads = std::thread::hardware_concurrency() - 2;
  params.add_parameter(numThreads, "--threads")
      .metavar("UNSIGNED INT")
      .absent(availableThreads)
      .help("Number of threads to raytrace with")
      .action([&](auto &target, const std::string &value) {
        int parsedValue = std::atoi(value.c_str());

        if (parsedValue <= 0) {
          std::cout << "WARNING: Invalid number of threads entered (" << value << "), using all available threads ("
                    << availableThreads << ")" << std::endl;
                    
          target = availableThreads;
        } else {
          target = parsedValue;
        }
      });



  if (!parser.parse_args(argc, argv, 1))
    std::exit(1);

  return std::make_tuple(imageWidth, imageHeight, editorWidth, editorHeight, pathToScene, numThreads);
}

int main(int argc, char **argv) {

  auto [imageWidth, imageHeight, editorWidth, editorHeight, pathToScene, numThreads] = setupArguments(argc, argv);

  // Image width is set but image height is not
  if (imageHeight == -1) {
    imageHeight = imageWidth;
  }

  if (pathToScene != "default") {
    rt::App app(imageWidth, imageHeight, editorWidth, editorHeight, pathToScene, numThreads);
    app.run();
  } else {
    rt::App app(imageWidth, imageHeight, editorWidth, editorHeight, rt::Scene::CornellBox(), numThreads);
    app.run();
  }

  return 0;
}
