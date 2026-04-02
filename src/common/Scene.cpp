#include "Scene.h"

using nlohmann::json;

namespace rt::raytracer {
  Scene::Scene(std::unique_ptr<AccelerationStructure> accelerationStructure, Camera camera, glm::ivec2 imageSize,
               vec3 backgroundColor)
      : worldRoot(std::move(accelerationStructure)), cam(camera), imageSize_(imageSize),
        backgroundColor_(backgroundColor) {}

  bool Scene::hit(rt::Ray const &worldspaceRay, float tMin, float tMax, rt::HitRecord &hitRecord) const {
    return worldRoot->hit(worldspaceRay, tMin, tMax, hitRecord);
  }

  vec3 Scene::backgroundColor(vec3 /*direction*/) const {
    // TODO sample skysphere
    // if (scene->skysphere)
    //   scene->skysphere->Hit(r, -rt::constants::infinity, rt::constants::infinity, rec);
    // else
    //   return scene->backgroundColor;
    return backgroundColor_;
  }

  auto Scene::imageSize() const -> glm::ivec2 { return imageSize_; }

  auto Scene::getRay(float u, float v) -> rt::Ray { return cam.GetRay(u, v); }
} // namespace rt::raytracer
