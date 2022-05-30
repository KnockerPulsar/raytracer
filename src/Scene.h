#pragma once
#include "../vendor/nlohmann-json/json.hpp"
#include "Camera.h"
#include "Defs.h"
#include "editor/Utils.h"
#include "textures/ImageTexture.h"
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <vector>

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
    ::Model skysphereModel;

    Camera cam;
    int    maxDepth, imageWidth, imageHeight, samplesPerPixel;
    vec3   backgroundColor;

    Scene() = default;
    Scene(Hittable *wr, Camera c, int md, int iw, int ih, int spp, vec3 bc)
        : worldRoot(wr), cam(c), maxDepth(md), imageWidth(iw), imageHeight(ih), samplesPerPixel(spp),
          backgroundColor(bc) {}

    void addSkysphere(std::string ssTex);

    void drawSkysphere();

    static Scene Scene1(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);

    static Scene Scene2(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);

    static Scene Random(int imageWidth,
                        int imageHeight,
                        int maxDepth,
                        int samplesPerPixel,
                        int ballGridWidth  = 11,
                        int ballGridHeight = 11);

    static Scene RandomMovingSpheres(int imageWidth,
                                     int imageHeight,
                                     int maxDepth,
                                     int samplesPerPixel,
                                     int ballGridWidth  = 11,
                                     int ballGridHeight = 11);

    static Scene TwoSpheres(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);

    static Scene Earth(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);

    static Scene Light(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);

    static Scene CornellBox(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);

    static Scene Load(std::string path);

    static Scene TransformationTest(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);

    static Scene PlaneTest(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);
    static Scene RasterTest(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel);

    json GetObjArray() const {
      std::vector<json> objJsons;

      // for (auto obj : objects.objects) {
      //   json temp = obj->GetJson();
      //   std::cout << temp << std::endl;
      //   objJsons.push_back(temp);
      // }
      return objJsons;
    }
  };

  inline void to_json(json &j, const Scene &s) {
    json objArr = s.GetObjArray();
    j           = json{{"settings",
                        {
                            {"resolution", {{"x", s.imageWidth}, {"y", s.imageHeight}}},
                            {"background_color", s.backgroundColor},
                            {"num_samples", s.samplesPerPixel},
                            {"max_depth", s.maxDepth},
              }},
             s.cam,
             {"objects", objArr}};
  }
} // namespace rt