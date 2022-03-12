#pragma once
#include "Camera.h"
#include "CheckerTexture.h"
#include "HittableList.h"
#include "Material.h"

namespace raytracer {

  class Scene {

  public:
    HittableList world;
    Camera       cam;
    int          maxDepth, imageWidth, imageHeight, samplesPerPixel;

    Scene &UpdateRenderData(int maxDepth, int imageWidth, int imageHeight,
                            int samplesPerPixel);

    static Scene Scene1(float aspectRatio);

    static Scene Scene2(float aspectRatio);
    static Scene Random(float aspectRatio, int ballGridWidth = 11,
                        int ballGridHeight = 11);
    static Scene RandomMovingSpheres(float aspectRatio, int ballGridWidth = 11,
                                     int ballGridHeight = 11);

    static Scene TwoSpheres(float aspectRatio);

    static Scene Earth(float aspectRatio);
    
  };
} // namespace raytracer