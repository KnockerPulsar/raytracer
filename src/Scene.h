#pragma once
#include "Camera.h"
#include "HittableList.h"

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
  };
} // namespace raytracer