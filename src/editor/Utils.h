#pragma once

#include <glm/glm.hpp>
#include <imgui.h>
#include <raylib.h>

#include <string>

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

  auto FitIntoArea(ImVec2 availableArea, ImVec2 areaToFit) -> ImVec2;

} // namespace EditorUtils
