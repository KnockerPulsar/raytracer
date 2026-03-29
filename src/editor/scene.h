#pragma once

#include "IImguiDrawable.h"

#include "editor/hittable.h"
#include "editor/camera.h"

#include <glm/fwd.hpp>
#include <imgui.h>
#include <nlohmann-json/json.hpp>

#include <functional>
#include <string>
#include <vector>

namespace rt::editor {
class Camera;

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

  // auto getInitialCamera() const -> rt::Camera;
  auto setCamera(Camera const &cam) -> void;
    // scene->cam = camera.toSceneCamera();
    // scene->imageSize  = camera.imageSize();

  void addSkysphere(std::string ssTex);
  void drawSkysphere();

  nlohmann::json toJson() const;

  // Initialized in Scene.cpp to a list of scene names and loading functions
  static std::vector<std::pair<std::string, std::function<Scene(int, int)>>> builtInScenes;

private:
  Scene(std::vector<std::shared_ptr<Hittable>> &&world, Camera cam, glm::ivec2 imageSize, vec3 backgroundColor,
        RaytraceSettings settings = RaytraceSettings{});

  RaytraceSettings settings;
  std::vector<std::shared_ptr<Hittable>> world;
  Camera cam;
  vec3 backgroundColor;
  glm::ivec2  imageSize;
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
