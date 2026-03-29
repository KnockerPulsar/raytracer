#pragma once
#include "Camera.h"
#include "Defs.h"
#include "acceleration_structure.h"

#include <nlohmann-json/json.hpp>
#include <raylib.h>
#include <raymath.h>
#include <imgui.h>

#include <vector>

namespace rt::raytracer {
  class Hittable;

  class Scene {

  public:
    /*
      So, setting this as a share pointer was problematic when updating BVHs
      The issue was that the BVH update I'm using is rebuilding the BVH with the new object
      Then I discard the old BVH and set this as its replacement

      So I think what happened was that on discarding the old BVH root, the tree was destroyed
      (or at least the root node). Then when we constructed the new BVH, that node was already freed
      causing issues when trying to access other nodes below it.

      Can potentially cause a memory leak if it's not handled correctly.
    */
    std::unique_ptr<AccelerationStructure> worldRoot;

    sPtr<Hittable> skysphere;

    std::string skysphereTexture;
    ::Model     skysphereModel;

    Camera cam;
    glm::ivec2 imageSize;
    vec3      backgroundColor;

    Scene() = default;

    Scene(std::unique_ptr<AccelerationStructure> wr, Camera c, int md, glm::ivec2 is, int spp, vec3 bc)
        : worldRoot(std::move(wr)), cam(c), imageSize(is), backgroundColor(bc) {}

    Scene(std::unique_ptr<AccelerationStructure> wr, Camera c, glm::ivec2 is, vec3 bc)
        : worldRoot(std::move(wr)), cam(c), imageSize(is), backgroundColor(bc) {}

    std::vector<std::shared_ptr<Hittable>> GetObjects() const;
  };

  void to_json(json &j, const Scene &s);
} // namespace rt
