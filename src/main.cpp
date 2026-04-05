#include "app.h"

#include <argumentum/argparse.h>

#include <charconv>
#include <cstdlib>
#include <format>
#include <string>

/*
 TODO:
    Why does the BVHNode ctor take t0, t1 as inputs? Can we remove those?

    Add the ability to change material textures

    Maybe add a big `RENDER` and `CANCEL RENDER` for the editor and raytracer states?

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


    Re-implement incremental rendering with job queue chunks
      - Would require blitting whenever a thread finishes a chunk

    Configure clangd to format in a better way

    Clean up code and naming
      - Method naming convention jumps between PascalCase and camelCase

    Implement meshes properly
      - TODO: Add the ability to add/change/reload? models at runtime

    Update nlhomann-json
*/

// X is the right axis
// Y is the vertical (AKA Up) axis
// Z is the forward axis

using namespace argumentum;

CliConfig setupArguments(int argc, char **argv) {

  argument_parser parser = argument_parser{};

  auto const parseSize = [](const std::string& passed, std::string_view optionName)
  {
        auto const xPosition = passed.find('x');
        if (xPosition == std::string::npos)
          throw std::runtime_error(std::format(
              "Error while parsing `--{}`: Invalid format. Correct format is <width>x<height>.",
              optionName));

        auto const parse = [&](auto start, int end, std::string_view paramName) {
          int               intValue;
          const auto *const parseStart       = passed.data() + start;
          const auto *const parseEnd         = passed.data() + end;

          if(parseStart == parseEnd || (parseStart == parseEnd && parseStart == passed.data()))
            throw std::runtime_error(std::format(
                "Error while parsing `--{}`: Failed to parse {}. Value does not exist.", optionName, paramName));

          auto widthParseResult = std::from_chars(parseStart, parseEnd, intValue);

          if (widthParseResult.ptr != parseEnd)
            throw std::runtime_error(std::format("Error while parsing `--{}`: Failed to parse {}", optionName, paramName));

          return intValue;
        };

        auto width  = parse(0, xPosition, "width");
        auto height = parse(xPosition + 1, passed.length(), "height");

        return glm::ivec2{width, height};
  };

  CliConfig config;
  parser.config().program(argv[0]).description("Raytracer");
  parser.params().add_parameter(config.imageSize, "--image-size")
      .absent(config.imageSize)
      .maxargs(1)
      .metavar("<width>x<height>")
      .action([&](glm::ivec2 &toAssign, const std::string &passed) { toAssign = parseSize(passed, "image-size"); })
      .help("rendering resolution width in pixels");

  parser.params().add_parameter(config.editorSize, "--editor-size")
      .absent(config.editorSize)
      .maxargs(1)
      .metavar("<width>x<height>")
      .action([&](glm::ivec2 &toAssign, const std::string &passed) { toAssign = parseSize(passed, "editor-size"); })
      .help("Window resolution width in pixels");

  parser.params().add_parameter(config.pathToScene, "--scene")
      .absent("")
      .maxargs(1)
      .metavar("STRING PATH")
      .help("Path to scene json");

  parser.params().add_parameter(config.numThreads, "--threads")
      .absent(config.numThreads)
      .maxargs(1)
      .metavar("UNSIGNED INT")
      .help("Number of threads to raytrace with")
      .action([&](auto &target, const std::string &value) {
        int parsedValue = std::atoi(value.c_str());

        if (parsedValue <= 0) {
          std::cout << "WARNING: Invalid number of threads entered (" << value << "), using default number of threads ("
                    << target << ")" << '\n';
        } else {
          target = parsedValue;
        }
      });

  parser.params().add_parameter(config.potato_mode, "--potato")
      .maxargs(1)
      .metavar("BOOL")
      .absent(false)
      .help(
          "Potato mode renders scenes at a quarter of the resolution. Useful for debugging and developing on battery.");

  if (!parser.parse_args(argc, argv, 1))
    std::exit(1);

  if (config.potato_mode)
    // glm's ivec2 multiply-assign operator first converts the scalar to an
    // int. So 0.25f becomes zero. To get around this, let's cast it up to a
    // float vec, multiply, then assign.
    config.imageSize = glm::vec2(config.imageSize) * 0.25f;

  return config;
}

int main(int argc, char **argv) {

  auto cliConfig = setupArguments(argc, argv);

  rt::App app(cliConfig);
  app.run();

  return 0;
}
