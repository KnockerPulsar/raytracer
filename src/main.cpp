#include <argumentum/argparse.h>
#include "Scene.h"
#include "app.h"
/*
 TODO:
    
    Try applying transformations before rendering instead of transforming each ray depending on the object we're checking collision with
    	- For boxes, planes, triangles: Should be as easy as applying the transform to the vertices once and recalculating normals
	    - For spheres and parameteric shapes: might have to do runtime transformations on normals for example.

    Change ColorPicker3 calls to ColorEdit

    Automatic image saving on render complete
      - stbi should take care of this.

    Finish up scene (de)serialization
      - Saving and loading done
      - Need more testing
      - Need to export existing scenes out and test them
      - Need to filter out common field saving and loading into the base Hittable class.
      - Hittables don't load their name
  
    Add menu with built in scenes 

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
    rt::App app(imageWidth, imageHeight, rt::Scene::Scene1(imageWidth, imageHeight));
    app.run();
  }


  return 0;
}
