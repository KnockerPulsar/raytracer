#include "Scene.h"
#include "Material.h"
#include "Ray.h"
#include "Sphere.h"
#include "Util.h"
#include "Vec3.h"
#include <bits/types/FILE.h>
#include <memory>
#include <raylib.h>
#include <tuple>

namespace raytracer {

  Scene &Scene::UpdateRenderData(int maxDepth, int imageWidth, int imageHeight,
                                 int samplesPerPixel) {
    this->maxDepth        = maxDepth;
    this->imageWidth      = imageWidth;
    this->imageHeight     = imageHeight;
    this->samplesPerPixel = samplesPerPixel;

    return *this;
  }

  Scene Scene::Scene1(float aspectRatio) {
    Scene        s;
    HittableList world;
    Vec3         lookFrom    = Vec3(3, 2, 3);
    Vec3         lookAt      = Vec3(0, 0, 0);
    Vec3         vUp         = Vec3(0, 1, 0);
    float        distToFocus = 10.0f;
    float        aperature   = 0.001F;
    Vec3         moveDir     = Vec3(0.1f, 0, 0);
    int          fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aspectRatio, aperature,
               distToFocus);

    auto material_ground = make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5));
    auto material_left   = make_shared<Dielectric>(1.5);
    auto material_right  = make_shared<Metal>(Vec3(0.8, 0.6, 0.2), 0.1);

    world //
        .Add(make_shared<Sphere>(100.0, Vec3(0, -100.5, -1), material_ground))
        .Add(make_shared<Sphere>(0.5, Vec3(0, 0, -1.0), material_center))

        // These 2 spheres make a "hollow sphere"
        .Add(make_shared<Sphere>(0.5, Vec3(-1., 0, -1), material_left))
        .Add(make_shared<Sphere>(-0.45, Vec3(-1., 0, -1), material_left))

        .Add(make_shared<Sphere>(0.5, Vec3(1., 0, -1), material_right));

    s.world = world;
    s.cam   = cam;

    return s;
  } // namespace raytracer

  Scene Scene::Scene2(float aspectRatio) {
    Scene        s;
    float        R = cos(pi / 4);
    HittableList world;
    Vec3         lookFrom    = Vec3(13, 2, 3);
    Vec3         lookAt      = Vec3(0, 0, 0);
    Vec3         vUp         = Vec3(0, 1, 0);
    float        distToFocus = 10.0f;
    float        aperature   = 0.1F;
    Vec3         moveDir     = Vec3(0.1f, 0, 0);
    int          fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aspectRatio, aperature,
               distToFocus);

    auto material_left  = make_shared<Lambertian>(Vec3(0, 0, 1));
    auto material_right = make_shared<Lambertian>(Vec3(1, 0, 0));

    world //
        .Add(make_shared<Sphere>(R, Vec3(-R, 0, -1), material_left))
        .Add(make_shared<Sphere>(R, Vec3(R, 0, -1), material_right));

    s.world = world;
    s.cam   = cam;

    return s;
  }

  Scene Scene::Random(float aspectRatio, int ballGridWidth,
                      int ballGridHeight) {
    Scene        s;
    HittableList world;

    Vec3  lookFrom    = Vec3(13, 2, 5);
    Vec3  lookAt      = Vec3(0, 0, 0);
    Vec3  vUp         = Vec3(0, 1, 0);
    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    Vec3  moveDir     = Vec3(1.0f, 0, 0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aspectRatio, aperature,
               distToFocus);

    auto groundMaterial = make_shared<Lambertian>(Vec3(0.5f));
    world.Add(make_shared<Sphere>(1000, Vec3(0, -1000, 0), groundMaterial));

    for (int a = -ballGridWidth; a < ballGridWidth; a++) {
      for (int b = -ballGridHeight; b < ballGridHeight; b++) {
        float chooseMat = RandomFloat();
        Vec3  center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());
        if ((center - Vec3(4, 0.2, 0)).Len() > 0.9) {
          shared_ptr<Material> sphereMaterial;

          if (chooseMat < 0.8) {
            // Diffuse
            auto albedo    = Vec3::Random() * Vec3::Random();
            sphereMaterial = make_shared<Lambertian>(albedo);
          } else if (chooseMat < 0.95) {
            // Metal
            auto  albedo   = Vec3::Random(0.5, 1);
            float fuzz     = RandomFloat(0, 0.5);
            sphereMaterial = make_shared<Metal>(albedo, fuzz);
          } else {
            sphereMaterial = make_shared<Dielectric>(1.5);
          }
          world.Add(make_shared<Sphere>(0.2, center, sphereMaterial));
        }
      }
    }
    auto mat1 = make_shared<Dielectric>(1.5);
    world.Add(make_shared<Sphere>(1.0, Vec3(0, 1, 0), mat1));

    auto mat2 = make_shared<Lambertian>(Vec3(0.4, 0.2, 0.1));
    world.Add(make_shared<Sphere>(1.0, Vec3(-4, 1, 0), mat2));

    auto mat3 = make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Sphere>(1.0, Vec3(4, 1, 0), mat3));

    s.world = world;
    s.cam   = cam;
    return s;
  }
} // namespace raytracer