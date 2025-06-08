#include "Utils.h"

#include <glm/gtc/constants.hpp>
#include <raylib.h>

#include <algorithm>
#include <cstdlib>
#include <format>
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

  ::Mesh generateSkysphere(float radius, const glm::ivec2 &segments) {
    Mesh mesh = {0};

    std::vector<float> *verts, *textCoords, *normals;
    verts = new std::vector<float>();
    textCoords = new std::vector<float>();
    normals = new std::vector<float>();

    std::vector<unsigned short> *elements = new std::vector<unsigned short>();

    // We populate the sphere vertices by looping over its longitude and
    // latitude
    for (int lat = 0; lat <= segments.y; lat++) {
      float v = (float)lat / segments.y;
      float pitch = v * glm::pi<float>() - glm::half_pi<float>();
      float cos = glm::cos(pitch), sin = glm::sin(pitch);
      for (int lng = 0; lng <= segments.x; lng++) {
        float u = (float)lng / segments.x;
        float yaw = u * glm::two_pi<float>();

        glm::vec3 normal =
            glm::vec3{cos * glm::cos(yaw), sin, cos * glm::sin(yaw)} * radius;
        glm::vec3 position = normal;
        glm::vec2 tex_coords = glm::vec2(u, v);
        ::Color color = {255, 255, 255, 255};

        verts->push_back(position.x);
        verts->push_back(position.y);
        verts->push_back(position.z);

        textCoords->push_back(tex_coords.x);
        textCoords->push_back(tex_coords.y);

        normals->push_back(normal.x);
        normals->push_back(normal.y);
        normals->push_back(normal.z);
      }
    }

    for (int lat = 1; lat <= segments.y; lat++) {
      int start = lat * (segments.x + 1);
      for (int lng = 1; lng <= segments.x; lng++) {
        int prev_lng = lng - 1;
        elements->push_back(lng + start);
        elements->push_back(lng + start - segments.x - 1);
        elements->push_back(prev_lng + start - segments.x - 1);
        elements->push_back(prev_lng + start - segments.x - 1);
        elements->push_back(prev_lng + start);
        elements->push_back(lng + start);
      }
    }

    mesh.vertices = verts->data();
    mesh.texcoords = textCoords->data();
    mesh.normals = normals->data();
    mesh.indices = elements->data();
    mesh.triangleCount = elements->size() / 3;
    mesh.vertexCount = verts->size() / 3;

    // ::Mesh kak = GenMeshSphere(500, 8, 8);

    UploadMesh(&mesh, false);

    return mesh;
  }

  auto FitIntoArea(ImVec2 availableArea, ImVec2 areaToFit) -> ImVec2 {
    auto const scale = std::min(availableArea.x / areaToFit.x, availableArea.y / areaToFit.y);
    return {areaToFit.x * scale, areaToFit.y * scale};
  }

} // namespace EditorUtils
