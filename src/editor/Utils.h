#pragma once

#include <raylib.h>
#include "../../vendor/glm/glm/glm.hpp"
#include "../../vendor/glm/glm/gtc/constants.hpp"
#include <imgui.h>

#include <stdexcept>
#include <string>
#include <memory>
#include <sys/types.h>
#include <tuple>
#include <vector>

using std::vector, std::tuple;

class Vector3;
class Vector2;
class Color;
class BoundingBox;

namespace EditorUtils {
  std::string Vector3ToStr(Vector3 vec);
  std::string Vector2ToStr(Vector2 vec);

  std::string GetIDFromPointer(void *address);

  // Create a sphere (the vertex order in the triangles are CCW from the outside)
  // Segments define the number of divisions on the both the latitude and the longitude
  ::Mesh generateSkysphere(float radius, const glm::ivec2 &segments);

  auto FitIntoArea(ImVec2 availableArea, ImVec2 textureSize) -> ImVec2;

} // namespace EditorUtils
