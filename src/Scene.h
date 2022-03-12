#pragma once
#include "Camera.h"
#include "CheckerTexture.h"
#include "HittableList.h"
#include "Material.h"
#include "../vendor/nlohmann-json/json.hpp"

namespace raytracer {

  class Scene {

  public:
    HittableList world;
    Camera       cam;
    int          maxDepth, imageWidth, imageHeight, samplesPerPixel;
    Vec3         backgroundColor;

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
  };
} // namespace raytracer