#pragma once
#include "../vendor/nlohmann-json/json.hpp"
#include "Camera.h"
#include "Defs.h"
#include "IImguiDrawable.h"
#include "Transformation.h"
#include "editor/Utils.h"
#include "textures/ImageTexture.h"
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <vector>

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

namespace rt {
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
    Hittable *worldRoot;

    sPtr<Hittable> skysphere;

    std::string skysphereTexture;
    ::Model     skysphereModel;

    Camera cam;
    int    imageWidth, imageHeight;
    vec3   backgroundColor;

    RaytraceSettings settings;

    // Initialized in Scene.cpp to a list of scene names and loading functions
    static std::vector<std::pair<std::string, std::function<Scene(int, int)>>> builtInScenes;

    Scene() = default;
    Scene(Hittable *wr, Camera c, int md, int iw, int ih, int spp, vec3 bc)
        : worldRoot(wr), cam(c), imageWidth(iw), imageHeight(ih), settings{spp, md}, backgroundColor(bc) {}

    Scene(Hittable *wr, Camera c, int iw, int ih, vec3 bc)
        : worldRoot(wr), cam(c), imageWidth(iw), imageHeight(ih), backgroundColor(bc) {}

    void addSkysphere(std::string ssTex);

    void drawSkysphere();

    static Scene Default(int imageWidth, int imageHeight);

    static Scene Scene1(int imageWidth, int imageHeight);

    static Scene Scene2(int imageWidth, int imageHeight);

    static Scene Random(int imageWidth, int imageHeight, int ballGridWidth = 11, int ballGridHeight = 11);

    static Scene RandomMovingSpheres(int imageWidth, int imageHeight, int ballGridWidth = 11, int ballGridHeight = 11);

    static Scene TwoSpheres(int imageWidth, int imageHeight);

    static Scene Earth(int imageWidth, int imageHeight);

    static Scene Light(int imageWidth, int imageHeight);

    static Scene CornellBox(int imageWidth, int imageHeight);

    static Scene Load(int imageWidth, int imageHeight, std::string path);

    static Scene TransformationTest(int imageWidth, int imageHeight);

    static Scene PlaneTest(int imageWidth, int imageHeight);
    static Scene RasterTest(int imageWidth, int imageHeight);

    json GetObjArray() const;

    json toJson() const;
  };

  inline void to_json(json &j, const Scene &s) {
    json objArr = s.GetObjArray();
    j           = json{
        {"settings",
                   {
                       {"background_color", s.backgroundColor},
                       {"num_samples", s.settings.samplesPerPixel},
                       {"max_depth", s.settings.maxDepth},
         }},
        s.cam,
        {"objects", objArr}};
  }
} // namespace rt