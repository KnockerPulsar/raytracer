#pragma once
#include <memory>
#include <optional>
#include <raylib.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <tuple>
#include <vector>
#include "../../vendor/glm/glm/glm.hpp"
#include "../../vendor/glm/glm/gtc/constants.hpp"

using std::vector, std::tuple;

class Vector3;
class Vector2;
class Color;
class BoundingBox;

namespace EditorUtils {
  class CameraEx;

  // 0: X, 1: Y, 2: Z
  std::tuple<Vector2, Vector2, Color>
  GetHUDAxis(CameraEx &cam, Vector2 screenDims, Vector2 offsetPcnts, float axisLength, int axis);

  std::tuple<Vector3, Vector3, Color>
  GetAxisAtPoint(Vector3 pos, Vector2 screenDims, float axisLength, float axisThickness, int axis);

  // vector<Cuboid> GenerateTestWorld(int maxCols);

  // void DrawPickedObjectAxes(const Camera &camera, const Cuboid &pickedCube, float screenWidth, float screenHeight);

  static std::string GetIDFromPointer(void *address) { return std::to_string((u_int64_t)address); }

  // Borrowed from https://stackoverflow.com/a/26221725
  template <typename... Args> std::string string_format(const std::string &format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0) {
      throw std::runtime_error("Error during formatting.");
    }
    auto                    size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
  }

  // Create a sphere (the vertex order in the triangles are CCW from the outside)
  // Segments define the number of divisions on the both the latitude and the longitude
  static ::Mesh generateSkysphere(float radius, const glm::ivec2 &segments) {
    Mesh mesh = {0};

    std::vector<float> *verts, *textCoords, *normals;
    verts      = new std::vector<float>();
    textCoords = new std::vector<float>();
    normals    = new std::vector<float>();

    std::vector<unsigned short> *elements = new std::vector<unsigned short>();

    // We populate the sphere vertices by looping over its longitude and latitude
    for (int lat = 0; lat <= segments.y; lat++) {
      float v     = (float)lat / segments.y;
      float pitch = v * glm::pi<float>() - glm::half_pi<float>();
      float cos = glm::cos(pitch), sin = glm::sin(pitch);
      for (int lng = 0; lng <= segments.x; lng++) {
        float u   = (float)lng / segments.x;
        float yaw = u * glm::two_pi<float>();

        glm::vec3 normal     = glm::vec3{cos * glm::cos(yaw), sin, cos * glm::sin(yaw)} * radius;
        glm::vec3 position   = normal;
        glm::vec2 tex_coords = glm::vec2(u, v);
        ::Color   color      = {255, 255, 255, 255};

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

    mesh.vertices  = verts->data();
    mesh.texcoords = textCoords->data();
    mesh.normals   = normals->data();
    mesh.indices   = elements->data();
    mesh.triangleCount = elements->size()/3;
    mesh.vertexCount = verts->size()/3;

    // ::Mesh kak = GenMeshSphere(500, 8, 8);

    UploadMesh(&mesh, false);

    return mesh;
  }
} // namespace EditorUtils