#pragma once
#include "Camera.h"
#include "Defs.h"
#include "acceleration_structure.h"

#include <nlohmann-json/json.hpp>
#include <raylib.h>
#include <raymath.h>
#include <imgui.h>

namespace rt::raytracer {
  class Hittable;

  class Scene {
  public:
    Scene(std::unique_ptr<AccelerationStructure> accelerationStructure, Camera camera, glm::ivec2 imageSize,
          vec3 backgroundColor);

    bool hit(rt::Ray const& worldspaceRay, float tMin, float tMax, rt::HitRecord& hitRecord) const;
    vec3 backgroundColor(vec3 direction) const;

    auto imageSize() const -> glm::ivec2;
    auto getRay(float u, float v) -> rt::Ray;

  private:
    std::unique_ptr<AccelerationStructure> worldRoot;

    sPtr<Hittable> skysphere;

    std::string skysphereTexture;
    ::Model     skysphereModel;

    Camera     cam;
    glm::ivec2 imageSize_;
    vec3       backgroundColor_;
  };

  void to_json(json &j, const Scene &s);
} // namespace rt
