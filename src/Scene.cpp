#include "Scene.h"
#include "BVHNode.h"
#include "Camera.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableBuilder.h"
#include "HittableList.h"
#include "Ray.h"
#include "Transformation.h"
#include "Util.h"
#include "data_structures/vec3.h"

#include "materials/Dielectric.h"
#include "materials/DiffuseLight.h"
#include "materials/Lambertian.h"
#include "materials/Material.h"
#include "materials/Metal.h"

#include "objects/MovingSphere.h"
#include "objects/ObjectFactory.h"
#include "objects/Plane.h"
#include "objects/Sphere.h"
#include "objects/Triangle.h"

#include "textures/CheckerTexture.h"
#include "textures/ImageTexture.h"
#include "textures/NoiseTexture.h"

#include <raylib.h>

#include <fstream>
#include <memory>
#include <ostream>
#include <vector>

using nlohmann::json;

namespace rt {

  std::vector<std::pair<std::string, std::function<Scene(int, int)>>> Scene::builtInScenes = {
      {"Default", Default},
      {"Scene1", Scene1},
      {"Scene2", Scene2},
      {"Random", std::bind(Random, std::placeholders::_1, std::placeholders::_2, 10, 10)},
      {"Random Moving", std::bind(RandomMovingSpheres, std::placeholders::_1, std::placeholders::_2, 10, 10)},
      {"TwoSpheres", TwoSpheres},
      {"Earth", Earth},
      {"Light", Light},
      {"Cornell", CornellBox},
      {"Transformation test", TransformationTest},
      {"Plane test", PlaneTest},
      {"Raster test", RasterTest}};

  void Scene::addSkysphere(std::string ssTex) {

    skysphereTexture = ssTex;

    auto tex = std::make_shared<ImageTexture>(ssTex.c_str(), false, true);

    auto skysphereMat = std::make_shared<DiffuseLight>(tex);
    skysphere         = std::make_shared<Sphere>(500.0f, skysphereMat);

    ::Image img = LoadImage(skysphereTexture.c_str());
    ImageFlipVertical(&img);

    skysphereModel = LoadModelFromMesh(EditorUtils::generateSkysphere(500, {32, 32}));
    skysphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTextureFromImage(img);
  }

  void Scene::drawSkysphere() {
    rlDisableBackfaceCulling();
    rlDisableDepthMask();
    DrawModel(skysphereModel, skysphere->transformation.getTranslation(), 1.0f, WHITE);
    rlEnableDepthMask();
    rlEnableBackfaceCulling();
  }

  Scene Scene::Default(int imageWidth, int imageHeight) {
    Scene        s;
    vec3         lookFrom        = vec3(3, 3, -3);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0);
    vec3         backgroundColor = vec3::Zero();

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(10.0f, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto mat   = make_shared<Lambertian>(vec3(0.6, 0.6, 0.6));
    auto light = make_shared<DiffuseLight>(vec3(3, 3, 3));

    const std::vector<std::shared_ptr<Hittable>> world = {
        HittableBuilder<Box>(1).withMaterial(mat),
        HittableBuilder<Plane>::CreateHittable(2.0, 2.0)
            .withTranslation(vec3(2, 0, 2))
            .withRotation(vec3(0, -45, 90))
            .withName("light plane")
            .withMaterial(light),
    };

    s = Scene(new BVHNode(world, 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);

    return s;
  }

  Scene Scene::Scene1(int imageWidth, int imageHeight) {
    Scene        s;
    vec3         lookFrom        = vec3(-1, 1, 1);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0);
    vec3         backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(10.0f, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto materialGround = make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
    auto materialCenter = make_shared<Lambertian>(vec3(0.1, 0.2, 0.5));
    auto materialLeft   = make_shared<Dielectric>(1.5);
    auto materialRight  = make_shared<Metal>(vec3(0.8, 0.6, 0.2), 0.1);

    auto groundSphere                     = Sphere(100.0, materialGround);
    groundSphere.transformation.setTranslation(vec3(0, -100.5, -1));
    groundSphere.name                     = "Ground Sphere";

    auto centerSphere                     = Sphere(0.5, materialCenter);
    centerSphere.transformation.setTranslation(vec3(0, 0, -1.0));
    centerSphere.name                     = "Center Sphere";

    auto leftSphereOuter                     = Sphere(0.5, materialLeft);
    leftSphereOuter.transformation.setTranslation(vec3(-1, 0, -1));
    leftSphereOuter.name                     = "Left Sphere Outer";
    auto leftSphereInner                     = Sphere(-0.45, materialLeft);
    leftSphereInner.transformation.setTranslation(vec3(-1, 0, -1));
    leftSphereInner.name                     = "Left Sphere Inner";
    auto rightSphere                         = Sphere(0.5, materialRight);
    rightSphere.transformation.setTranslation(vec3(1, 0, -1));
    rightSphere.name                         = "Right Sphere";

    const std::vector<std::shared_ptr<Hittable>> world = {
        make_shared<Sphere>(groundSphere),
        make_shared<Sphere>(centerSphere),
        // These 2 spheres make a "hollow sphere"
        make_shared<Sphere>(leftSphereOuter),
        make_shared<Sphere>(leftSphereInner),
        make_shared<Sphere>(rightSphere),
    };

    s = Scene(new BVHNode(world, 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);

    return s;
  } // namespace rt

  Scene Scene::Scene2(int imageWidth, int imageHeight) {
    Scene        s;
    float        r = cos(rt::constants::pi / 4);
    vec3         lookFrom        = vec3(13, 2, 3);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0);
    vec3         backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(0.1f, 0, 0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto materialLeft  = make_shared<Lambertian>(vec3(0, 0, 1));
    auto materialRight = make_shared<Lambertian>(vec3(1, 0, 0));

    auto leftSphere                      = make_shared<Sphere>(Sphere(r, materialLeft));
    leftSphere->transformation.setTranslation(vec3(-r, 0, -1));

    auto rightSphere                      = make_shared<Sphere>(Sphere(r, materialRight));
    rightSphere->transformation.setTranslation(vec3(r, 0, -1));

    const std::vector<std::shared_ptr<Hittable>> world = {leftSphere, rightSphere};

    s = Scene(new BVHNode(world, 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);

    return s;
  }

  Scene Scene::Random(int imageWidth, int imageHeight, int ballGridWidth, int ballGridHeight) {
    Scene        s;

    vec3 lookFrom        = vec3(13, 2, 5);
    vec3 lookAt          = vec3(0, 0, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(1.0f, 0, 0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    std::vector<std::shared_ptr<Hittable>> world = {
        HittableBuilder<Sphere>(1.0, make_shared<Dielectric>(1.5)).withTranslation(vec3(0, 1, 0)),
        HittableBuilder<Sphere>(1.0, make_shared<Lambertian>(vec3(0.4, 0.2, 0.1)))
            .withTranslation(vec3(-4, 1, 0))
            ,
        HittableBuilder<Sphere>(1.0, make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0))
            .withTranslation(vec3(4, 1, 0))
            ,
        HittableBuilder<Sphere>(1000, make_shared<Lambertian>(vec3(0.5f))).withTranslation(vec3(0, -1000, 0)),
    };

    for (int a = -ballGridWidth; a < ballGridWidth; a++) {
      for (int b = -ballGridHeight; b < ballGridHeight; b++) {
        float chooseMat = RandomFloat();
        vec3  center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());
        if ((center - vec3(4, 0.2, 0)).Len() > 0.9) {
          shared_ptr<Material> sphereMaterial;

          if (chooseMat < 0.8) {
            // Diffuse
            auto albedo    = vec3::Random() * vec3::Random();
            sphereMaterial = make_shared<Lambertian>(albedo);
          } else if (chooseMat < 0.95) {
            // Metal
            auto  albedo   = vec3::Random(0.5, 1);
            float fuzz     = RandomFloat(0, 0.5);
            sphereMaterial = make_shared<Metal>(albedo, fuzz);
          } else {
            sphereMaterial = make_shared<Dielectric>(1.5);
          }

          auto sphere                     = Sphere(0.2, sphereMaterial);
          sphere.transformation.setTranslation(center);
          world.push_back(make_shared<Sphere>(sphere));
        }
      }
    }

    s = Scene(new BVHNode(world, 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);

    return s;
  }

  Scene Scene::RandomMovingSpheres(int imageWidth, int imageHeight, int ballGridWidth, int ballGridHeight) {
    Scene        s;

    vec3 lookFrom        = vec3(13, 2, 3);
    vec3 lookAt          = vec3(0, 0, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(1.0f, 0, 0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus, 0.0, 1.0);

    auto checker = std::make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));

    auto groundMaterial                   = make_shared<Lambertian>(vec3(0.5f));

    std::vector<std::shared_ptr<Hittable>> world = {
        HittableBuilder<Sphere>(Sphere(1000, make_shared<Lambertian>(checker)))
            .withTranslation(vec3(0, -1000, 0))
            ,
        HittableBuilder<Sphere>(1.0, make_shared<Dielectric>(1.5)).withTranslation(vec3(0, 1, 0)),
        HittableBuilder<Sphere>(1.0, make_shared<Lambertian>(vec3(0.4, 0.2, 0.1)))
            .withTranslation(vec3(-4, 1, 0))
            ,
        HittableBuilder<Sphere>(1.0, make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0))
            .withTranslation(vec3(4, 1, 0))
            ,
    };

    for (int a = -ballGridWidth; a < ballGridWidth; a++) {
      for (int b = -ballGridHeight; b < ballGridHeight; b++) {
        float chooseMat = RandomFloat();
        vec3  center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());
        if ((center - vec3(4, 0.2, 0)).Len() > 0.9) {
          shared_ptr<Material> sphereMaterial;

          if (chooseMat < 0.8) {
            // Diffuse
            auto albedo    = vec3::Random() * vec3::Random();
            sphereMaterial = make_shared<Lambertian>(albedo);
            vec3 center2   = center + vec3(0, RandomFloat(0, .5), 0);
            world.push_back(make_shared<MovingSphere>(center, center2, 0.0, 1.0, 0.2, sphereMaterial));
          } else if (chooseMat < 0.95) {
            // Metal
            auto  albedo   = vec3::Random(0.5, 1);
            float fuzz     = RandomFloat(0, 0.5);
            sphereMaterial = make_shared<Metal>(albedo, fuzz);

            auto sphere                     = Sphere(0.2, sphereMaterial);
            sphere.transformation.setTranslation(center);
            world.push_back(make_shared<Sphere>(sphere));

          } else {
            sphereMaterial                  = make_shared<Dielectric>(1.5);
            auto sphere                     = Sphere(0.2, sphereMaterial);
            sphere.transformation.setTranslation(center);
            world.push_back(make_shared<Sphere>(sphere));
          }
        }
      }
    }

    s = Scene(new BVHNode(world, 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);

    return s;
  }

  Scene Scene::TwoSpheres(int imageWidth, int imageHeight) {
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    Camera cam(
        vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0), 20.0, (float)imageWidth / imageHeight, 0.0, 10, 0, 0
    );

    Scene s;

    // auto checker =
    //     make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9,
    //     0.9));

    auto perlinTexture  = make_shared<NoiseTexture>(4.0f);
    auto checkerTexture = make_shared<CheckerTexture>(vec3(0), vec3(1), 6);

    sPtr<Material> lambertianPerlin  = make_shared<Lambertian>(perlinTexture);
    sPtr<Material> lambertianChecker = make_shared<Lambertian>(checkerTexture);

    std::vector<std::shared_ptr<Hittable>> world = {
        HittableBuilder<Sphere>(10).withTranslation(vec3(0, -10, 0)).withMaterial(lambertianPerlin),
        HittableBuilder<Sphere>(10).withTranslation(vec3(0, 10, 0)).withMaterial(lambertianChecker)};

    return Scene(new HittableList(std::move(world)), cam, {imageWidth, imageHeight}, backgroundColor);
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

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus, 0.0, 1.0);

    auto earthTexture = make_shared<ImageTexture>("assets/textures/earthmap.png");
    auto earthSurface = make_shared<Lambertian>(earthTexture);

    std::vector<std::shared_ptr<Hittable>> world = {make_shared<Sphere>(2, earthSurface)};

    return Scene(new HittableList(std::move(world)), cam, {imageWidth, imageHeight}, backgroundColor);
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

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus, 0.0, 1.0);

    auto perText   = make_shared<NoiseTexture>(4.0f);
    auto diffLight = make_shared<DiffuseLight>(vec3(4, 4, 4));

    std::vector<std::shared_ptr<Hittable>> world = {
        HittableBuilder<Sphere>(1000, make_shared<Lambertian>(perText)).withTranslation(vec3(0, -1000, 0)),
        HittableBuilder<Sphere>(2, make_shared<Lambertian>(perText)).withTranslation(vec3(0, 2, 0)),
        HittableBuilder<XYRect>(3, 5, 1, 3, -2, diffLight),
    };

    return Scene(new HittableList(std::move(world)), cam, {imageWidth, imageHeight}, backgroundColor);
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

    Camera cam(
        lookFrom, lookAt, vUp, fov, float(imageWidth) / float(imageHeight), aperature, distToFocus, 0.0, 1.0
    );

    auto red           = make_shared<Lambertian>(vec3(0.65, 0.05, 0.05));
    auto white         = make_shared<Lambertian>(vec3(0.73, 0.73, 0.73));
    auto green         = make_shared<Lambertian>(vec3(0.12, 0.45, 0.15));
    auto light         = make_shared<DiffuseLight>(vec3(2, 2, 2));
    auto purplishMetal = make_shared<Metal>(vec3(0.8, 0.1, 0.8), 0.7);
    auto chrome        = make_shared<Metal>(vec3(0.8, 0.8, 0.8), 0.05);
    auto dielectric    = make_shared<Dielectric>(1.5f);
    auto noise         = make_shared<Lambertian>(make_shared<NoiseTexture>(0.1f));
    auto checker       = std::make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(10, 3.0, 10), 0.2f);

    float wallWidth  = 55.5;
    float wallHeight = wallWidth;

    std::vector<std::shared_ptr<Hittable>> world = {
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
    };

    return Scene(new BVHNode(world, 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);
  }

  Scene Scene::Load(glm::ivec2 imageSize, std::string path) {
    std::ifstream jsonFile(path);
    std::cout << "Loading scene from file " << path << '\n';

    json readScene;
    jsonFile >> readScene;

    json  settings    = readScene["settings"];
    float aspectRatio = static_cast<float>(imageSize.x) / imageSize.y;
    Scene s           = Scene(
        nullptr,
        Camera(readScene["camera"], aspectRatio),
        settings["max_depth"].get<int>(),
        imageSize,
        settings["num_samples"].get<int>(),
        settings["background_color"].get<vec3>()
    );

    auto world = std::vector<std::shared_ptr<Hittable>>(readScene["objects"].size());

    for (const auto &obj : readScene["objects"]) {
      auto objPtr = ObjectFactory::FromJson(obj);
      if (objPtr)
        world.push_back(objPtr);
    }

    std::cout << "Loaded scene with\n"
              << "\tsettings " << settings << '\n'
              << "\t#objects " << world.size() << '\n';

    s.worldRoot = new BVHNode(world, s.cam.time0, s.cam.time1);

    return s;
  }

  Scene Scene::TransformationTest(int imageWidth, int imageHeight) {

    Scene s;
    vec3  lookFrom        = vec3(0, 0, 0);
    vec3  lookAt          = vec3(0, 0, -1);
    vec3  vUp             = vec3(0, 1, 0);
    vec3  backgroundColor = vec3(0.0, 0.0, 0.0);

    float distToFocus = 10.0f;
    float aperature   = 0.0;
    vec3  moveDir     = vec3(0.0f, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    const auto *const skyspherePath = "assets/textures/skysphere.png";
    auto        sphereMat     = make_shared<DiffuseLight>(make_shared<ImageTexture>(skyspherePath, true));

    std::vector<sPtr<Hittable>> world = {
        HittableBuilder<Box>(1)
            .withMaterial(make_shared<Metal>(vec3(0.8), 0.1f))
            .withTranslation(vec3(0, 0, -5))
            .withName("Cube"),
        HittableBuilder<Sphere>(0.5f, sphereMat).withName("Sphere"),
    };

    s = Scene(new BVHNode(world, 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);
    s.addSkysphere(skyspherePath);

    return s;
  }

  Scene Scene::PlaneTest(int imageWidth, int imageHeight) {
    Scene        s;
    vec3         lookFrom        = vec3(0, 3, 0);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0.1);
    vec3         backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(0, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto red = make_shared<DiffuseLight>(vec3(3, 0, 0));
    auto magenta =
        make_shared<DiffuseLight>(make_shared<CheckerTexture>(vec3(3, 0, 3), vec3::Zero(), 40));
    auto blue = make_shared<DiffuseLight>(make_shared<CheckerTexture>(vec3(0, 0, 3), vec3::Zero(), 40));

    auto plane = HittableBuilder<Plane>(3, 3).withMaterial(magenta).withTranslation(vec3(-3, 10, 0));
    auto xzplane = make_shared<XZRect>(-1.5, 1.5, -1.5, 1.5, 0, blue);
    auto tri     = HittableBuilder<Triangle>(vec3(0, 0, 1), vec3(-1, 0, -1), vec3(1, 0, 0)).withMaterial(red);

    const std::vector<std::shared_ptr<Hittable>> world = {tri, plane, xzplane};

    return Scene(new BVHNode(world, 0, 1), cam, {imageWidth, imageHeight}, backgroundColor);
  } // namespace rt

  Scene Scene::RasterTest(int imageWidth, int imageHeight) {
    Scene s;
    vec3  lookFrom = vec3(4, 4, 4);
    vec3  lookAt   = vec3(0, 0, 0);
    vec3  vUp      = vec3(0, 1, 0.1);
    // vec3         backgroundColor = vec3(0.70, 0.80, 1.00);
    vec3 backgroundColor = vec3(0.0, 0.0, 0.00);

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(0, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    std::vector<std::shared_ptr<Hittable>> world = {
        HittableBuilder<Box>(vec3(1, 1, 1), vec3(2, 2, 2)).withMaterial(make_shared<DiffuseLight>(vec3(3, 0, 0)))};

    return Scene(new HittableList{std::move(world)}, cam, {imageWidth, imageHeight}, backgroundColor);
  }

  std::vector<std::shared_ptr<Hittable>> Scene::GetObjects() const { return worldRoot->getChildrenAsList(); }

  json Scene::toJson() const {
    json sceneJson;
    to_json(sceneJson, *this);
    return sceneJson;
  }

  void to_json(json &j, const Scene &s)
  {
    auto const        objectList = s.GetObjects();
    std::vector<json> objJsons(objectList.size());

    for (const auto &obj : objectList) {
      objJsons.push_back(obj->toJson());
    }

    j = json{{"settings",
              {
                  {"background_color", s.backgroundColor},
                  {"num_samples", s.settings.samplesPerPixel},
                  {"max_depth", s.settings.maxDepth},
              }},
             s.cam,
             {"objects", objJsons}};
  }
} // namespace rt
