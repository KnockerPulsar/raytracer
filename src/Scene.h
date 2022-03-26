#pragma once
#include "../vendor/nlohmann-json/json.hpp"
#include "Camera.h"
#include "HittableList.h"
#include "materials/Material.h"
#include "textures/CheckerTexture.h"

namespace rt {

  class Scene {

  public:
    HittableList world;
    Camera       cam;
    int          maxDepth, imageWidth, imageHeight, samplesPerPixel;
    vec3         backgroundColor;

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

    // static json Save(std::string path);
  };
} // namespace rt