#include "scene.h"

#include "common/Camera.h"
#include "common/Raytracable.h"
#include "common/Util.h"
#include "common/bvh_tree.h"
#include "common/Scene.h"

#include "hittatable_builder.h"
#include "camera.h"
#include "object_factory.h"
#include "background.h"
#include "solid_color_background.h"
#include "skysphere_background.h"

#include "data_structures/vec3.h"

#include "objects/box.h"
#include "objects/plane.h"
#include "objects/sphere.h"
#include "objects/mesh.h"

#include "materials/lambertian.h"
#include "materials/diffuse_light.h"
#include "materials/dielectric.h"
#include "materials/metal.h"

#include "textures/noise_texture.h"
#include "textures/checker_texture.h"
#include "textures/image_texture.h"

#include <functional>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <utility>

namespace rt::editor {
  std::vector<std::pair<std::string, std::function<std::unique_ptr<Scene>(int, int)>>> Scene::builtInScenes = {
      {"Default", [](int w, int h) { return std::make_unique<Scene>(Scene::Default(w, h)); }},
      {"Scene1", [] (int w, int h) { return std::make_unique<Scene>(Scene::Scene1(w, h)); }},
      {"Scene2", [] (int w, int h) { return std::make_unique<Scene>(Scene::Scene2(w, h)); }},
      {"Random", [] (int w, int h) { return std::make_unique<Scene>(Scene::Random(w, h, 10, 10)); }},
      // {"Random Moving", std::bind(RandomMovingSpheres, std::placeholders::_1, std::placeholders::_2, 10, 10)},
      {"TwoSpheres", [] (int w, int h) { return std::make_unique<Scene>(Scene::TwoSpheres(w, h)); }},
      {"Earth", [] (int w, int h) { return std::make_unique<Scene>(Scene::Earth(w, h)); }},
      {"Light", [] (int w, int h) { return std::make_unique<Scene>(Scene::Light(w, h)); }},
      {"Cornell", [] (int w, int h) { return std::make_unique<Scene>(Scene::CornellBox(w, h)); }},
      // {"Transformation test", TransformationTest},
      // {"Plane test", PlaneTest},
      // {"Raster test", RasterTest},
  };

  Scene Scene::Default(int imageWidth, int imageHeight) {
    vec3  lookFrom        = vec3(3, 3, -3);
    vec3  lookAt          = vec3(0, 0, 0);
    vec3  backgroundColor = vec3::Zero();

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(10.0f, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, fov, aperature, distToFocus);

    auto mat   = std::make_shared<Lambertian>(vec3(0.6, 0.6, 0.6));
    auto light = std::make_shared<DiffuseLight>(vec3(3, 3, 3));

    std::vector<std::shared_ptr<Object>> world = {
        HittableBuilder<Box>(1).withMaterial(mat).withName("Box"),
        HittableBuilder<Plane>(2.0, 2.0)
            .withTranslation(vec3(2, 0, 2))
            .withRotation(vec3(0, -45, 90))
            .withName("light plane")
            .withMaterial(light),
    };

    return Scene(std::move(world), cam, std::make_unique<SolidColorBackground>(backgroundColor));
  }

Scene Scene::Scene1(int imageWidth, int imageHeight) {
  vec3  lookFrom        = vec3(-1, 1, 1);
  vec3  lookAt          = vec3(0, 0, 0);
  vec3  backgroundColor = vec3(0.70, 0.80, 1.00);

  float distToFocus = 10.0f;
  float aperature   = 0.001F;
  vec3  moveDir     = vec3(10.0f, 0, 0);
  int   fov         = 60.0;

  Camera cam(lookFrom, lookAt, fov, aperature, distToFocus);

  auto materialGround = std::make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
  auto materialCenter = std::make_shared<Lambertian>(vec3(0.1, 0.2, 0.5));
  auto materialLeft   = std::make_shared<Dielectric>(1.5);
  auto materialRight  = std::make_shared<Metal>(vec3(0.8, 0.6, 0.2), 0.1);

  auto groundSphere = HittableBuilder<Sphere>(100.0)
                          .withTranslation(vec3(0, -100.5, -1))
                          .withMaterial(materialGround)
                          .withName("Ground Sphere");

  auto centerSphere = HittableBuilder<Sphere>(0.5)
                          .withTranslation(vec3(0, 0, -1.0))
                          .withMaterial(materialCenter)
                          .withName("Center Sphere");

  auto leftSphereOuter = HittableBuilder<Sphere>(0.5)
                             .withTranslation(vec3(-1, 0, -1))
                             .withMaterial(materialLeft)
                             .withName("Left Sphere Outer");

  auto leftSphereInner = HittableBuilder<Sphere>(-0.45)
                             .withTranslation(vec3(-1, 0, -1))
                             .withMaterial(materialLeft)
                             .withName("Left Sphere Inner");

  auto rightSphere =
      HittableBuilder<Sphere>(0.5).withTranslation(vec3(1, 0, -1)).withMaterial(materialRight).withName("Right Sphere");

  std::vector<std::shared_ptr<Object>> world = {
      groundSphere,
      centerSphere,
      // These 2 spheres make a "hollow sphere"
      leftSphereOuter,
      leftSphereInner,
      rightSphere,
  };

  return Scene(std::move(world), cam, std::make_unique<SolidColorBackground>(backgroundColor));
} // namespace rt

Scene Scene::Scene2(int imageWidth, int imageHeight) {
  float r               = cos(rt::constants::pi / 4);
  vec3  lookFrom        = vec3(13, 2, 3);
  vec3  lookAt          = vec3(0, 0, 0);
  vec3  vUp             = vec3(0, 1, 0);
  vec3  backgroundColor = vec3(0.70, 0.80, 1.00);

  float distToFocus = 10.0f;
  float aperature   = 0.1F;
  vec3  moveDir     = vec3(0.1f, 0, 0);
  int   fov         = 20.0;

  Camera cam(lookFrom, lookAt, fov, aperature, distToFocus);

  auto materialLeft  = std::make_shared<Lambertian>(vec3(0, 0, 1));
  auto materialRight = std::make_shared<Lambertian>(vec3(1, 0, 0));

  std::vector<std::shared_ptr<Object>> world = {
      HittableBuilder<Sphere>(r).withTranslation(vec3(-r, 0, -1)).withMaterial(materialLeft).withName("Left"),
      HittableBuilder<Sphere>(r).withTranslation(vec3(r, 0, -1)).withMaterial(materialRight).withName("Right"),
  };

  return Scene(std::move(world), cam, std::make_unique<SolidColorBackground>(backgroundColor));
}

Scene Scene::Random(int imageWidth, int imageHeight, int ballGridWidth, int ballGridHeight) {
  vec3              lookFrom         = vec3(13, 2, 5);
  vec3              lookAt           = vec3(0, 0, 0);
  vec3              vUp              = vec3(0, 1, 0);
  const auto *const skyspherePath = "assets/textures/skysphere.png";

  float distToFocus = 10.0f;
  float aperature   = 0.1F;
  vec3  moveDir     = vec3(1.0f, 0, 0);
  int   fov         = 20.0;

  Camera cam(lookFrom, lookAt, fov, aperature, distToFocus);

  std::vector<std::shared_ptr<Object>> world = {
      HittableBuilder<Sphere>(1.0)
          .withTranslation(vec3(0, 1, 0))
          .withMaterial(std::make_shared<Dielectric>(1.5))
          .withName("Glass Sphere"),
      HittableBuilder<Sphere>(1.0)
          .withTranslation(vec3(-4, 1, 0))
          .withMaterial(std::make_shared<Lambertian>(vec3(0.4, 0.2, 0.1)))
          .withName("Diffuse Sphere"),
      HittableBuilder<Sphere>(1.0)
          .withTranslation(vec3(4, 1, 0))
          .withMaterial(std::make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0))
          .withName("Metal Sphere"),
      HittableBuilder<Sphere>(1000)
          .withTranslation(vec3(0, -1000, 0))
          .withMaterial(std::make_shared<Lambertian>(vec3(0.5f)))
          .withName("Ground"),
  };

  for (int a = -ballGridWidth; a < ballGridWidth; a++) {
    for (int b = -ballGridHeight; b < ballGridHeight; b++) {
      float chooseMat = RandomFloat();
      vec3  center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());
      if ((center - vec3(4, 0.2, 0)).Len() > 0.9) {

        auto const [material, materialName] = [&chooseMat] -> std::pair<std::shared_ptr<Material>, std::string> {
          if (chooseMat < 0.8) {
            // Diffuse
            auto albedo = vec3::Random() * vec3::Random();
            return {std::make_shared<Lambertian>(albedo), "Diffuse"};
          } else if (chooseMat < 0.95) {
            // Metal
            auto  albedo = vec3::Random(0.5, 1);
            float fuzz   = RandomFloat(0, 0.5);
            return {std::make_shared<Metal>(albedo, fuzz), "Metal"};
          } else {
            return {std::make_shared<Dielectric>(1.5), "Glass"};
          }
        }();

        auto const smallSphere = HittableBuilder<Sphere>(0.2).withTranslation(center).withMaterial(material).withName(
            std::format("Small {} Sphere", materialName));

        world.push_back(smallSphere);
      }
    }
  }

  return Scene(std::move(world), cam, std::make_unique<SkysphereBackground>(skyspherePath));
}

// Scene Scene::RandomMovingSpheres(int imageWidth, int imageHeight, int ballGridWidth, int ballGridHeight) {
//   Scene        s;
//
//   vec3 lookFrom        = vec3(13, 2, 3);
//   vec3 lookAt          = vec3(0, 0, 0);
//   vec3 vUp             = vec3(0, 1, 0);
//   vec3 backgroundColor = vec3(0.70, 0.80, 1.00);
//
//   float distToFocus = 10.0f;
//   float aperature   = 0.1F;
//   vec3  moveDir     = vec3(1.0f, 0, 0);
//   int   fov         = 20.0;
//
//   Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus, 0.0, 1.0);
//
//   auto checker = std::make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));
//
//   auto groundMaterial                   = std::make_shared<Lambertian>(vec3(0.5f));
//
//   std::vector<std::shared_ptr<Hittable>> world = {
//       HittableBuilder<Sphere>(Sphere(1000, std::make_shared<Lambertian>(checker)))
//           .withTranslation(vec3(0, -1000, 0))
//           ,
//       HittableBuilder<Sphere>(1.0, std::make_shared<Dielectric>(1.5)).withTranslation(vec3(0, 1, 0)),
//       HittableBuilder<Sphere>(1.0, std::make_shared<Lambertian>(vec3(0.4, 0.2, 0.1)))
//           .withTranslation(vec3(-4, 1, 0))
//           ,
//       HittableBuilder<Sphere>(1.0, std::make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0))
//           .withTranslation(vec3(4, 1, 0))
//           ,
//   };
//
//   for (int a = -ballGridWidth; a < ballGridWidth; a++) {
//     for (int b = -ballGridHeight; b < ballGridHeight; b++) {
//       float chooseMat = RandomFloat();
//       vec3  center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());
//       if ((center - vec3(4, 0.2, 0)).Len() > 0.9) {
//         shared_ptr<Material> sphereMaterial;
//
//         if (chooseMat < 0.8) {
//           // Diffuse
//           auto albedo    = vec3::Random() * vec3::Random();
//           sphereMaterial = std::make_shared<Lambertian>(albedo);
//           vec3 center2   = center + vec3(0, RandomFloat(0, .5), 0);
//           world.push_back(std::make_shared<MovingSphere>(center, center2, 0.0, 1.0, 0.2, sphereMaterial));
//         } else if (chooseMat < 0.95) {
//           // Metal
//           auto  albedo   = vec3::Random(0.5, 1);
//           float fuzz     = RandomFloat(0, 0.5);
//           sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
//
//           auto sphere                     = Sphere(0.2, sphereMaterial);
//           sphere.transformation.setTranslation(center);
//           world.push_back(std::make_shared<Sphere>(sphere));
//
//         } else {
//           sphereMaterial                  = std::make_shared<Dielectric>(1.5);
//           auto sphere                     = Sphere(0.2, sphereMaterial);
//           sphere.transformation.setTranslation(center);
//           world.push_back(std::make_shared<Sphere>(sphere));
//         }
//       }
//     }
//   }
//
//   return Scene(std::make_unique<BVHTree>(std::move(world), 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);
// }

Scene Scene::TwoSpheres(int imageWidth, int imageHeight) {
  vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

  Camera cam(vec3(13, 2, 3), vec3(0, 0, 0), 20.0, 0.0, 10);


  // auto checker =
  //     std::make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9,
  //     0.9));

  auto perlinTexture  = std::make_shared<NoiseTexture>(4.0f);
  auto checkerTexture = std::make_shared<CheckerTexture>(vec3(0), vec3(1), 6);

  sPtr<Material> lambertianPerlin  = std::make_shared<Lambertian>(perlinTexture);
  sPtr<Material> lambertianChecker = std::make_shared<Lambertian>(checkerTexture);

  std::vector<std::shared_ptr<Object>> world = {
      HittableBuilder<Sphere>(10)
          .withTranslation(vec3(0, -10, 0))
          .withMaterial(lambertianPerlin)
          .withName("Perlin Sphere"),
      HittableBuilder<Sphere>(10)
          .withTranslation(vec3(0, 10, 0))
          .withMaterial(lambertianChecker)
          .withName("Checker Sphere"),
  };

  return Scene(std::move(world), cam, std::make_unique<SolidColorBackground>(backgroundColor));
}

Scene Scene::Earth(int imageWidth, int imageHeight) {
  vec3 lookFrom        = vec3(13, 2, 3);
  vec3 lookAt          = vec3(0, 0, 0);
  vec3 vUp             = vec3(0, 1, 0);
  vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

  float distToFocus = 10.0f;
  float aperature   = 0.1F;
  vec3  moveDir     = vec3(0.0f, 0, 0.0);
  int   fov         = 20.0;

  Camera cam(lookFrom, lookAt, fov, aperature, distToFocus);

  auto earthTexture = std::make_shared<ImageTexture>("assets/textures/earthmap.png");
  auto earthSurface = std::make_shared<Lambertian>(earthTexture);

  std::vector<std::shared_ptr<Object>> world = {
      HittableBuilder<Sphere>(2).withMaterial(earthSurface).withName("Earth")};

  return Scene(std::move(world), cam, std::make_unique<SolidColorBackground>(backgroundColor));
}

Scene Scene::Light(int imageWidth, int imageHeight) {
  vec3 lookFrom        = vec3(26, 3, 6);
  vec3 lookAt          = vec3(0, 2, 0);
  vec3 vUp             = vec3(0, 1, 0);
  vec3 backgroundColor = vec3::Zero();

  float distToFocus = 10.0f;
  float aperature   = 0.1F;
  vec3  moveDir     = vec3(0.0f, 0, 0.0);
  int   fov         = 20.0;

  Camera cam(lookFrom, lookAt, fov, aperature, distToFocus);

  auto perText   = std::make_shared<NoiseTexture>(4.0f);
  auto perMat    = std::make_shared<Lambertian>(perText);
  auto diffLight = std::make_shared<DiffuseLight>(vec3(4, 4, 4));

  std::vector<std::shared_ptr<Object>> world = {
      HittableBuilder<Sphere>(1000).withTranslation(vec3(0, -1000, 0)).withMaterial(perMat).withName("Floor Sphere"),
      HittableBuilder<Sphere>(2).withTranslation(vec3(0, 2, 0)).withMaterial(perMat).withName("Sphere"),
      HittableBuilder<Plane>(2, 2)
          .withTranslation(vec3(2, 2, -2))
          .withRotation(vec3(90, 0, 0))
          .withMaterial(diffLight)
          .withName("Light"),
  };

  return Scene(std::move(world), cam, std::make_unique<SolidColorBackground>(backgroundColor));
}

Scene Scene::CornellBox(int imageWidth, int imageHeight) {
  vec3 lookFrom        = vec3(27.8, 27.8, -80);
  vec3 lookAt          = vec3(27.8, 27.8, 0);
  vec3 vUp             = vec3(0, 1, 0);
  vec3 backgroundColor = vec3(0.2, 0.2, 0.2);

  float distToFocus = 80.0f;
  float aperature   = 0.1f;
  vec3  moveDir     = vec3(0.0f, 0, 0.0);
  int   fov         = 40.0;

  Camera cam(lookFrom, lookAt, fov, aperature, distToFocus);

  auto red           = std::make_shared<Lambertian>(vec3(0.65, 0.05, 0.05));
  auto white         = std::make_shared<Lambertian>(vec3(0.73, 0.73, 0.73));
  auto green         = std::make_shared<Lambertian>(vec3(0.12, 0.45, 0.15));
  auto light         = std::make_shared<DiffuseLight>(vec3(2, 2, 2));
  auto purplishMetal = std::make_shared<Metal>(vec3(0.8, 0.1, 0.8), 0.7);
  auto chrome        = std::make_shared<Metal>(vec3(0.8, 0.8, 0.8), 0.05);
  auto gold          = std::make_shared<Metal>(vec3(0.831, 0.686, 0.215), 0.05);
  auto dielectric    = std::make_shared<Dielectric>(1.5f);
  auto noise         = std::make_shared<Lambertian>(std::make_shared<NoiseTexture>(0.1f));
  auto checker       = std::make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(10, 3.0, 10), 0.2f);

  float wallWidth  = 55.5;
  float wallHeight = wallWidth;

  std::vector<std::shared_ptr<Object>> world = {
      HittableBuilder<Plane>(wallHeight, wallHeight)
          .withTranslation(vec3(wallWidth, wallWidth / 2, wallHeight / 2))
          .withRotation(vec3(0, 0, 90))
          .withMaterial(green)
          .withName("left wall"),
      HittableBuilder<Plane>(wallWidth, wallHeight)
          .withTranslation(vec3(0, wallWidth / 2, wallHeight / 2))
          .withRotation(vec3(0, 0, -90))
          .withMaterial(red)
          .withName("right wall"),
      HittableBuilder<Plane>(wallWidth, wallHeight)
          .withTranslation(vec3(wallWidth / 2, wallHeight, wallHeight / 2))
          .withRotation(vec3(0, 0, 180))
          .withMaterial(light)
          .withName("light"),
      HittableBuilder<Plane>(wallWidth, wallHeight)
          .withTranslation(vec3(wallWidth / 2, 0, wallHeight / 2))
          .withMaterial(chrome)
          .withName("floor"),
      HittableBuilder<Plane>(wallWidth, wallHeight)
          .withTranslation(vec3(wallWidth / 2, wallHeight / 2, wallWidth))
          .withRotation(vec3(0, 90, -90))
          .withMaterial(white)
          .withName("back wall"),
      HittableBuilder<Box>(vec3(16.5, 33, 16.5))
          .withTranslation(vec3(40, 16.5, 40))
          .withRotation(vec3(0, 15, 0))
          .withMaterial(white)
          .withName("Box 1"),
      HittableBuilder<Box>(vec3(10))
          .withTranslation(vec3(10, 5, 10))
          .withRotation(vec3(0, 12, 0))
          .withMaterial(purplishMetal)
          .withName("Box 2"),
      HittableBuilder<Mesh>("assets/models/nefertiti-5k.obj")
          .withMaterial(gold)
          .withTranslation(vec3(10, 10, 10)), // On top of box 2
  };

  return Scene(std::move(world), cam, std::make_unique<SolidColorBackground>(backgroundColor));
}

Scene Scene::Load(std::string path) {
  std::ifstream jsonFile(path);
  std::cout << "Loading scene from file " << path << '\n';

  json readScene;
  jsonFile >> readScene;

  auto world = std::vector<std::shared_ptr<Object>>();
  world.reserve(readScene["objects"].size());

  for (const auto &obj : readScene["objects"]) {
    auto objPtr = ObjectFactory::FromJson(obj);
    if (objPtr)
      world.push_back(objPtr);
  }

  auto const camera = [&] {
    auto const cameraJson = readScene["camera"];
    return Camera(cameraJson["look_from"].get<vec3>(), cameraJson["look_at"].get<vec3>(),
                  cameraJson["fov"].get<float>(), cameraJson["aperature"].get<float>(),
                  cameraJson["focus_dist"].get<float>());
  }();

  auto const settings         = readScene["settings"];
  // auto const raytraceSettings = [&] {
  //   return Config{settings["max_depth"].get<int>(), settings["num_samples"].get<int>()};
  // }();

  // Differentiate skysphere and solid color backgrounds
  Scene s =
      Scene(std::move(world), camera, std::make_unique<SolidColorBackground>(settings["background_color"].get<vec3>()));

  std::cout << "Loaded scene with\n"
            << "\tsettings " << settings << '\n'
            << "\t#objects " << world.size() << '\n';

  return s;
}

// Scene Scene::TransformationTest(int imageWidth, int imageHeight) {
//
//   vec3  lookFrom        = vec3(0, 0, 0);
//   vec3  lookAt          = vec3(0, 0, -1);
//   vec3  vUp             = vec3(0, 1, 0);
//   vec3  backgroundColor = vec3(0.0, 0.0, 0.0);
//
//   float distToFocus = 10.0f;
//   float aperature   = 0.0;
//   vec3  moveDir     = vec3(0.0f, 0, 0);
//   int   fov         = 60.0;
//
//   Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);
//
//   const auto *const skyspherePath = "assets/textures/skysphere.png";
//   auto              sphereMat     = std::make_shared<DiffuseLight>(std::make_shared<ImageTexture>(skyspherePath, true));
//
//   std::vector<sPtr<Hittable>> world = {
//       HittableBuilder<Box>(1)
//           .withMaterial(std::make_shared<Metal>(vec3(0.8), 0.1f))
//           .withTranslation(vec3(0, 0, -5))
//           .withName("Cube"),
//       HittableBuilder<Sphere>(0.5f, sphereMat).withName("Sphere"),
//   };
//
//   // s.addSkysphere(skyspherePath); // TODO
//
//   return Scene(std::move(world), cam, {imageWidth, imageHeight}, backgroundColor);
// }

// Scene Scene::PlaneTest(int imageWidth, int imageHeight) {
//   vec3  lookFrom        = vec3(0, 3, 0);
//   vec3  lookAt          = vec3(0, 0, 0);
//   vec3  vUp             = vec3(0, 1, 0.1);
//   vec3  backgroundColor = vec3(0.70, 0.80, 1.00);
//
//   float distToFocus = 10.0f;
//   float aperature   = 0.001F;
//   vec3  moveDir     = vec3(0, 0, 0);
//   int   fov         = 60.0;
//
//   Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);
//
//   auto red     = std::make_shared<DiffuseLight>(vec3(3, 0, 0));
//   auto magenta = std::make_shared<DiffuseLight>(make_shared<CheckerTexture>(vec3(3, 0, 3), vec3::Zero(), 40));
//   auto blue    = std::make_shared<DiffuseLight>(make_shared<CheckerTexture>(vec3(0, 0, 3), vec3::Zero(), 40));
//
//   auto plane   = HittableBuilder<Plane>(3, 3).withMaterial(magenta).withTranslation(vec3(-3, 10, 0));
//   // auto xzplane = std::make_shared<XZRect>(-1.5, 1.5, -1.5, 1.5, 0, blue);
//   auto tri     = HittableBuilder<Triangle>(vec3(0, 0, 1), vec3(-1, 0, -1), vec3(1, 0, 0)).withMaterial(red);
//
//   std::vector<std::shared_ptr<Hittable>> world = {tri, plane, xzplane};
//   return Scene(std::make_unique<BVHTree>(std::move(world), 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);
// } // namespace rt

// Scene Scene::RasterTest(int imageWidth, int imageHeight) {
//   Scene s;
//   vec3  lookFrom = vec3(4, 4, 4);
//   vec3  lookAt   = vec3(0, 0, 0);
//   vec3  vUp      = vec3(0, 1, 0.1);
//   // vec3         backgroundColor = vec3(0.70, 0.80, 1.00);
//   vec3 backgroundColor = vec3(0.0, 0.0, 0.00);
//
//   float distToFocus = 10.0f;
//   float aperature   = 0.001F;
//   vec3  moveDir     = vec3(0, 0, 0);
//   int   fov         = 60.0;
//
//   Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);
//
//   std::vector<std::shared_ptr<Hittable>> world = {
//       HittableBuilder<Box>(vec3(1, 1, 1), vec3(2, 2, 2)).withMaterial(std::make_shared<DiffuseLight>(vec3(3, 0, 0)))};
//
//   return Scene(std::make_unique<HittableList>(std::move(world)), cam, {imageWidth, imageHeight}, backgroundColor);
// }

// void Scene::addSkysphere(std::string ssTex) {
//
//   skysphereTexture = ssTex;
//
//   auto tex = std::make_shared<ImageTexture>(ssTex.c_str(), false, true);
//
//   auto skysphereMat = std::make_shared<DiffuseLight>(tex);
//   skysphere         = std::make_shared<Sphere>(500.0f, skysphereMat);
//
//   ::Image img = LoadImage(skysphereTexture.c_str());
//   ImageFlipVertical(&img);
//
//   skysphereModel = LoadModelFromMesh(EditorUtils::generateSkysphere(500, {32, 32}));
//   skysphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTextureFromImage(img);
// }
//
// void Scene::drawSkysphere() {
//   rlDisableBackfaceCulling();
//   rlDisableDepthMask();
//   DrawModel(skysphereModel, skysphere->transformation.getTranslation(), 1.0f, WHITE);
//   rlEnableDepthMask();
//   rlEnableBackfaceCulling();
// }
//
// json Scene::toJson() const {
//   json sceneJson;
//   to_json(sceneJson, *this);
//   return sceneJson;
// }

Scene::Scene(std::vector<std::shared_ptr<Object>> &&world, Camera cam, std::unique_ptr<Background> background)
    : world(std::move(world)), cam(cam), background(std::move(background)) {}

auto Scene::getWorld() -> std::vector<std::shared_ptr<Object>> & { return world; }

auto Scene::compile(rt::Camera const &camera) const -> std::unique_ptr<raytracer::Scene> {
  std::vector<std::unique_ptr<rt::Raytracable>> raytracables;
  raytracables.reserve(world.size());

  for (auto const &obj : world) {
    raytracables.push_back(obj->toRaytracable());
  }

  return std::make_unique<raytracer::Scene>(std::make_unique<raytracer::BVHTree>(std::move(raytracables), 0, 1), camera,
                                            background->toRaytracerBackground());
}

Scene::Camera::Camera(vec3 lookFrom, vec3 lookAt, float fov, float aperature, float focusDist)
    : lookFrom(lookFrom), lookAt(lookAt), vFov(fov), aperature(aperature),
      focusDist_(focusDist) {}

} // namespace rt::editor
