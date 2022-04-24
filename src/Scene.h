#pragma once
#include "../vendor/nlohmann-json/json.hpp"
#include "BVHNode.h"
#include "Camera.h"
#include "Defs.h"
#include "HittableList.h"
#include "materials/Dielectric.h"
#include "materials/Material.h"
#include "textures/CheckerTexture.h"
#include <vector>

namespace rt {

  class Scene {

  public:
    HittableList world;   // Might contain a BVH, harder to serialize
    HittableList objects; // Should always contain a linear list of object
                          // that's easy to iterate over
    Camera cam;
    int    maxDepth, imageWidth, imageHeight, samplesPerPixel;
    vec3   backgroundColor;

    static Scene Scene1(int imageWidth, int imageHeight, int maxDepth,
                        int samplesPerPixel);

    static Scene Scene2(int imageWidth, int imageHeight, int maxDepth,
                        int samplesPerPixel);

    static Scene Random(int imageWidth, int imageHeight, int maxDepth,
                        int samplesPerPixel, int ballGridWidth = 11,
                        int ballGridHeight = 11);

    static Scene RandomMovingSpheres(int imageWidth, int imageHeight,
                                     int maxDepth, int samplesPerPixel,
                                     int ballGridWidth  = 11,
                                     int ballGridHeight = 11);

    static Scene TwoSpheres(int imageWidth, int imageHeight, int maxDepth,
                            int samplesPerPixel);

    static Scene Earth(int imageWidth, int imageHeight, int maxDepth,
                       int samplesPerPixel);

    static Scene Light(int imageWidth, int imageHeight, int maxDepth,
                       int samplesPerPixel);

    static Scene CornellBox(int imageWidth, int imageHeight, int maxDepth,
                            int samplesPerPixel);

    static Scene Load(std::string path);

    static Scene TransformationTest(int imageWidth, int imageHeight,
                                    int maxDepth, int samplesPerPixel);
    static Scene PlaneTest(int imageWidth, int imageHeight, int maxDepth,
                           int samplesPerPixel);

    json GetObjArray() const {
      std::vector<json> objJsons;

      for (auto obj : objects.objects) {
        json temp = obj->GetJson();
        std::cout << temp << std::endl;
        objJsons.push_back(temp);
      }
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