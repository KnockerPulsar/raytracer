#pragma once
#include "../vendor/nlohmann-json/json.hpp"
#include "Camera.h"
#include "Defs.h"
#include "IImguiDrawable.h"
#include "Transformation.h"
#include "editor/Utils.h"
#include "rt.h"
#include "textures/ImageTexture.h"
#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <tuple>
#include <vector>

using std::string, std::vector, std::function, std::pair;


namespace rt {
  class Hittable;

  class Scene {

  public:
    /*
      So, setting this as a shared pointer was problematic when updating BVHs
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

    vec3 backgroundColor = Defaults::backgroundColor;

    // Initialized in Scene.cpp to a list of scene names and loading functions
    static vector<pair<string, function<Scene()>>> builtInScenes;

    Scene() = default;
    Scene(Hittable *wr, Camera c, int md, int spp, vec3 bc)
        : worldRoot(wr), cam(c),  backgroundColor(bc) {}

    Scene(Hittable *wr, Camera c, vec3 bc)
        : worldRoot(wr), cam(c), backgroundColor(bc) {}

    void addSkysphere(std::string ssTex);
    void drawSkysphere();

    static Scene Default();
    static Scene Scene1();
    static Scene Scene2();
    static Scene Random(int ballGridWidth = Defaults::ballGridWidth, int ballGridHeight = Defaults::ballGridWidth);
    static Scene RandomMovingSpheres(int ballGridWidth = Defaults::ballGridWidth, int ballGridHeight = Defaults::ballGridWidth);
    static Scene TwoSpheres();
    static Scene Earth();
    static Scene Light();
    static Scene CornellBox();
    static Scene Load(std::string path);
    static Scene TransformationTest();
    static Scene PlaneTest();
    static Scene RasterTest();


    json GetObjArray() const;
    json toJson() const;
  };

  inline void to_json(json &j, const Scene &s) {
    json objArr = s.GetObjArray();

    // clang-format off
    j = json{
        s.cam,
        {"objects", objArr}
    };
    // clang-format off
  }
} // namespace rt