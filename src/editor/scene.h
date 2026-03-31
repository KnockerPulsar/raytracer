#pragma once

#include "IImguiDrawable.h"

#include "editor/hittable.h"

#include <glm/fwd.hpp>
#include <imgui.h>
#include <nlohmann-json/json.hpp>

#include <functional>
#include <string>
#include <vector>

namespace rt::editor {
class Camera;
class Object;

struct RaytraceSettings : public rt::IImguiDrawable {
  int samplesPerPixel = 1;
  int maxDepth        = 10;

  RaytraceSettings() = default;
  RaytraceSettings(int spp, int md) : samplesPerPixel(spp), maxDepth(md) {}

  void OnImgui() override {
    ImGui::Begin("Raytrace settings");
    ImGui::DragInt("Samples per pixel", &samplesPerPixel, 1, 1, 500);
    ImGui::DragInt("Maximum depth", &maxDepth, 1, 1, 100);
    ImGui::End();
  }
};

class Scene {
public:
  class Camera
  {
  public:
    Camera(Camera const &other) = default;
    Camera(vec3 lookFrom, vec3 lookAt, float vFov, float aspectRatio, float aperature,
           float focusDist);

    vec3  getLookFrom() const { return lookFrom; }
  protected:
    vec3  lookFrom, lookAt;
    float focusDist_;
    float aspectRatio;
    float vFov;
    float aperature;

    static inline const vec3 vUp = vec3(0, 1, 0); // TODO make customizable
  };

  // TODO: update these to accept glm::vec2
  static Scene Default(int imageWidth, int imageHeight);
  static Scene Scene1(int imageWidth, int imageHeight);
  static Scene Scene2(int imageWidth, int imageHeight);
  static Scene Random(int imageWidth, int imageHeight, int ballGridWidth = 11, int ballGridHeight = 11);
  static Scene RandomMovingSpheres(int imageWidth, int imageHeight, int ballGridWidth = 11, int ballGridHeight = 11);
  static Scene TwoSpheres(int imageWidth, int imageHeight);
  static Scene Earth(int imageWidth, int imageHeight);
  static Scene Light(int imageWidth, int imageHeight);
  static Scene CornellBox(int imageWidth, int imageHeight);
  static Scene Load(glm::ivec2 imageSize, std::string path);
  // static Scene TransformationTest(int imageWidth, int imageHeight);
  // static Scene PlaneTest(int imageWidth, int imageHeight);
  // static Scene RasterTest(int imageWidth, int imageHeight);

  auto getCamera() const -> Scene::Camera { return cam; }

  auto setCamera(Camera const &cam) -> void;
    // scene->cam = camera.toSceneCamera();
    // scene->imageSize  = camera.imageSize();
  auto getWorld() -> std::vector<std::shared_ptr<Object>>&;

  void addSkysphere(std::string ssTex);
  void drawSkysphere();

  // nlohmann::json toJson() const;

  auto imageSize() const -> glm::ivec2 { return imageSize_; }

  // Initialized in Scene.cpp to a list of scene names and loading functions
  static std::vector<std::pair<std::string, std::function<std::unique_ptr<Scene>(int, int)>>> builtInScenes;

private:
  Scene(std::vector<std::shared_ptr<Object>> &&world, Camera cam, glm::ivec2 imageSize, vec3 backgroundColor,
        RaytraceSettings settings = RaytraceSettings{});

  RaytraceSettings settings;
  std::vector<std::shared_ptr<Object>> world;
  Camera cam;
  vec3 backgroundColor;
  glm::ivec2  imageSize_;
};

// void to_json(json &j, const Scene &s) {
//   auto const        objectList = s.GetObjects();
//   std::vector<json> objJsons(objectList.size());
//
//   for (const auto &obj : objectList) {
//     objJsons.push_back(obj->toJson());
//   }
//
//   j = json{{"settings",
//             {
//                 {"background_color", s.backgroundColor},
//                 {"num_samples", s.settings.samplesPerPixel},
//                 {"max_depth", s.settings.maxDepth},
//             }},
//            s.cam,
//            {"objects", objJsons}};
// }
} // namespace rt::editor
