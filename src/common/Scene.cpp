#include "Scene.h"
#include "raytracer/background.h"

using nlohmann::json;

namespace rt::raytracer {
  Scene::Scene(std::unique_ptr<AccelerationStructure> accelerationStructure, Camera camera, glm::ivec2 imageSize,
               std::unique_ptr<Background> background)
      : worldRoot(std::move(accelerationStructure)), cam(camera), imageSize_(imageSize),
        background(std::move(background)) {}

  bool Scene::hit(rt::Ray const &worldspaceRay, float tMin, float tMax, rt::HitRecord &hitRecord) const {
    return worldRoot->hit(worldspaceRay, tMin, tMax, hitRecord);
  }

  vec3 Scene::backgroundColor(vec3 direction) const {
    return background->sampleBackground(direction);
  }

  auto Scene::imageSize() const -> glm::ivec2 { return imageSize_; }

  auto Scene::getRay(float u, float v) -> rt::Ray { return cam.GetRay(u, v); }
} // namespace rt::raytracer
