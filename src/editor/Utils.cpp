#include "Utils.h"

#include <glm/gtc/constants.hpp>
#include <raylib.h>

#include <algorithm>
#include <format>
#include <cstring>
#include <vector>

namespace EditorUtils {
  std::string Vector3ToStr(Vector3 vec) {
    return std::format("{{ {}, {}, {} }}", vec.x, vec.y, vec.z);
  }

  std::string Vector2ToStr(Vector2 vec) {
    return std::format("{{ {}, {} }}", vec.x, vec.y);
  }

  std::string GetIDFromPointer(void *address) {
    return std::to_string((u_int64_t)address);
  }

  ::Mesh generateSkysphere(float radius, int rings, int slices) {
    ::Mesh mesh = {0};

    // 1. Calculate counts
    int vertexCount   = (rings + 1) * (slices + 1);
    int triangleCount = rings * slices * 2;

    // 2. Build data in temporary vectors
    std::vector<float>          vertices;
    std::vector<float>          texcoords;
    std::vector<unsigned short> indices;

    vertices.reserve(vertexCount * 3);
    texcoords.reserve(vertexCount * 2);
    indices.reserve(triangleCount * 3);

    // 3. Generate Geometry
    for (int r = 0; r <= rings; r++) {
      float v = (float)r / rings;
      // theta: 0 to PI (latitude)
      float theta    = v * std::numbers::pi_v<float>;
      float sinTheta = std::sin(theta);
      float cosTheta = std::cos(theta);

      for (int s = 0; s <= slices; s++) {
        float u = (float)s / slices;
        // phi: 0 to 2PI (longitude)
        float phi = u * 2.0f * std::numbers::pi_v<float>;

        // Standard Spherical to Cartesian (Y-up)
        float x = radius * sinTheta * std::cos(phi);
        float y = radius * cosTheta; // Note: cos(theta) gives us the height
        float z = radius * sinTheta * std::sin(phi);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);

        texcoords.push_back(u);
        texcoords.push_back(v);
      }
    }

    // 4. Generate Indices (Clockwise winding for inside-out viewing)
    for (int r = 0; r < rings; r++) {
      for (int s = 0; s < slices; s++) {
        int first  = r * (slices + 1) + s;
        int second = first + slices + 1;

        // Triangle 1 (Reversed order for inward facing)
        indices.push_back(first);
        indices.push_back(first + 1);
        indices.push_back(second);

        // Triangle 2 (Reversed order for inward facing)
        indices.push_back(second);
        indices.push_back(first + 1);
        indices.push_back(second + 1);
      }
    }

    // 5. Transfer to Raylib-owned memory
    // Raylib's UnloadMesh() calls RL_FREE, so we must use RL_MALLOC or MemAlloc
    mesh.vertexCount   = vertexCount;
    mesh.triangleCount = triangleCount;

    mesh.vertices = (float *)MemAlloc(vertices.size() * sizeof(float));
    std::memcpy(mesh.vertices, vertices.data(), vertices.size() * sizeof(float));

    mesh.texcoords = (float *)MemAlloc(texcoords.size() * sizeof(float));
    std::memcpy(mesh.texcoords, texcoords.data(), texcoords.size() * sizeof(float));

    mesh.indices = (unsigned short *)MemAlloc(indices.size() * sizeof(unsigned short));
    std::memcpy(mesh.indices, indices.data(), indices.size() * sizeof(unsigned short));

    // 6. Upload to GPU
    UploadMesh(&mesh, false);

    return mesh;
  }

  auto FitIntoArea(ImVec2 availableArea, ImVec2 areaToFit) -> ImVec2 {
    auto const scale = std::min(availableArea.x / areaToFit.x, availableArea.y / areaToFit.y);
    return {areaToFit.x * scale, areaToFit.y * scale};
  }

} // namespace EditorUtils
