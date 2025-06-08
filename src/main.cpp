#include "Scene.h"
#include "app.h"

#include <argumentum/argparse.h>

#include <cstdlib>
#include <ostream>
#include <string>
#include <tuple>
/*
 TODO:
    Update CLI flags for image and editor width and height with new flags that accept "widthxheight"
      - Why are we using "foo_bar" and not "foo-bar"?

    Regenerate BVH tree automatically on object transform with ImGuizmo?
      - At least after letting go
      - Need a regeneration method instead of using addChild(nullptr)

    Add the ability to change material textures

    Finish up scene (de)serialization
      - Saving and loading done
      - Need more testing
      - Need to export existing scenes out and test them
      - Need to filter out common field saving and loading into the base Hittable class.
      - Hittables don't load their name

    For previewing procedural textures, could sample a small (50x50) preview from the texture
      - Compute shader might be able to make things faster at the cost of more complexity
        - Another approach would be to make the raytracer more "functional"
          - `frame = raytracer.render(scene, parameters)`
          - Need to set up a scene with the selected object and the material, with the object fully inside the camera
            frustum
      - Caching results is a must, can't calculate a preview each frame
      - Need to setup a way to rebuild previews on parameter change
      - Perlin noise is difficult to preview since it's 3D

    Can add a prepass before raytracing where triangles and AABBs are transformed
        Try applying transformations before rendering instead of transforming each ray depending on the object we're checking collision with
          - For boxes, planes, triangles: Should be as easy as applying the transform to the vertices once and recalculating normals
          - For spheres and parameteric shapes: might have to do runtime transformations on normals for example.
            - For example for spheres, on hit, we'd get the point and rotate it (or the normal?) based on the rotation component of the transformation

        Modify all hittables so that they're centered at the world origin to use scaling matrices.
          - Box, plane, sphere all done
          - Not sure about the rest


    Re-implement incremental rendering with job queue chunks
      - Would require blitting whenever a thread finishes a chunk

    Configure clangd to format in a better way

    Clean up code and naming
      - Method naming convention jumps between PascalCase and camelCase

    Implement meshes properly
      - Obj loading?
*/

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

using namespace argumentum;

CliConfig setupArguments(int argc, char **argv) {
  const int imageWidthDefault = 900;
  const int numThreadsDefault = 6;

  CliConfig config;

  argument_parser parser = argument_parser{};
  auto            params = parser.params();

  parser.config().program(argv[0]).description("Raytracer");
  parser.add_argument(config.imageWidth, "--image_width")
      .maxargs(1)
      .metavar("UNSIGNED INT")
      .absent(imageWidthDefault)
      .help("rendering resolution width in pixels");

  parser.add_argument(config.imageHeight, "--image_height")
      .maxargs(1)
      .metavar("UNSIGNED INT")
      .absent(-1)
      .help("rendering resolution width in pixels");

  parser.add_argument(config.editorWidth, "--editor_width")
      .maxargs(1)
      .metavar("UNSIGNED INT")
      .absent(config.editorWidth)
      .help("Window resolution width in pixels");

  parser.add_argument(config.editorHeight, "--editor_height")
      .maxargs(1)
      .metavar("UNSIGNED INT")
      .absent(config.editorHeight)
      .help("Window resolution width in pixels");

  parser.add_argument(config.pathToScene, "--scene")
      .maxargs(1)
      .metavar("STRING PATH")
      .absent("")
      .help("Path to scene json");

  parser.add_argument(config.numThreads, "--threads")
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


  // Image width is set but image height is not
  if (config.imageHeight == -1) {
    config.imageHeight = config.imageWidth;
  }

  return config;
}

int main(int argc, char **argv) {

  auto cliConfig = setupArguments(argc, argv);

  rt::App app(cliConfig);
  app.run();

  return 0;
}
