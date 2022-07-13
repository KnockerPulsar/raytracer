#include "Scene.h"
#include "AABB.h"
#include "BVHNode.h"
#include "Defs.h"
#include "Transformation.h"
#include "materials/DiffuseLight.h"

#include "Camera.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"
#include "Util.h"
#include "data_structures/vec3.h"
#include "materials/ConstantMedium.h"
#include "materials/Dielectric.h"
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
#include "textures/SolidColor.h"
#include "textures/Texture.h"
#include <bits/types/FILE.h>
#include <fstream>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <tuple>
#include <vector>

#include "HittableBuilder.h"

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
    DrawModel(skysphereModel, skysphere->transformation.translate, 1.0f, WHITE);
    rlEnableDepthMask();
    rlEnableBackfaceCulling();
  }

  Scene Scene::Default(int imageWidth, int imageHeight) {
    Scene        s;
    HittableList world;
    vec3         lookFrom        = vec3(3, 3, -3);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0);
    vec3         backgroundColor = vec3::Zero();

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(10.0f, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto mat   = make_shared<Lambertian>(vec3(0.6, 0.6, 0.6));
    auto light = make_shared<DiffuseLight>(vec3(3, 3, 3));

    world //
        .Add(HittableBuilder<Box>(1).withMaterial(mat).build())
        .Add(HittableBuilder<Plane>::CreateHittable(2.0, 2.0)
                 .withTranslation(vec3(2, 0, 2))
                 .withRotation(vec3(0, -45, 90))
                 .withName("light plane")
                 .withMaterial(light)
                 .build());

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::Scene1(int imageWidth, int imageHeight) {
    Scene        s;
    HittableList world;
    vec3         lookFrom        = vec3(-1, 1, 1);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0);
    vec3         backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(10.0f, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto materialGround = make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
    auto materialCenter = make_shared<Lambertian>(vec3(0.1, 0.2, 0.5));
    auto materialLeft   = make_shared<Dielectric>(1.5);
    auto materialRight  = make_shared<Metal>(vec3(0.8, 0.6, 0.2), 0.1);

    auto groundSphere                     = Sphere(100.0, materialGround);
    groundSphere.transformation.translate = vec3(0, -100.5, -1);
    groundSphere.name                     = "Ground Sphere";

    auto centerSphere                     = Sphere(0.5, materialCenter);
    centerSphere.transformation.translate = vec3(0, 0, -1.0);
    centerSphere.name                     = "Center Sphere";

    auto leftSphereOuter                     = Sphere(0.5, materialLeft);
    leftSphereOuter.transformation.translate = vec3(-1, 0, -1);
    leftSphereOuter.name                     = "Left Sphere Outer";
    auto leftSphereInner                     = Sphere(-0.45, materialLeft);
    leftSphereInner.transformation.translate = vec3(-1, 0, -1);
    leftSphereInner.name                     = "Left Sphere Inner";
    auto rightSphere                         = Sphere(0.5, materialRight);
    rightSphere.transformation.translate     = vec3(1, 0, -1);
    rightSphere.name                         = "Right Sphere";

    world //
        .Add(make_shared<Sphere>(groundSphere))
        .Add(make_shared<Sphere>(centerSphere))
        // These 2 spheres make a "hollow sphere"
        .Add(make_shared<Sphere>(leftSphereOuter))
        .Add(make_shared<Sphere>(leftSphereInner))
        .Add(make_shared<Sphere>(rightSphere));

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  } // namespace rt

  Scene Scene::Scene2(int imageWidth, int imageHeight) {
    Scene        s;
    float        r = cos(pi / 4);
    HittableList world;
    vec3         lookFrom        = vec3(13, 2, 3);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0);
    vec3         backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(0.1f, 0, 0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto materialLeft  = make_shared<Lambertian>(vec3(0, 0, 1));
    auto materialRight = make_shared<Lambertian>(vec3(1, 0, 0));

    auto leftSphere                      = make_shared<Sphere>(Sphere(r, materialLeft));
    leftSphere->transformation.translate = vec3(-r, 0, -1);

    auto rightSphere                      = make_shared<Sphere>(Sphere(r, materialRight));
    rightSphere->transformation.translate = vec3(r, 0, -1);

    world // Scene::Scene
        .Add(leftSphere)
        .Add(rightSphere);

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::Random(int imageWidth, int imageHeight, int ballGridWidth, int ballGridHeight) {
    Scene        s;
    HittableList world;

    vec3 lookFrom        = vec3(13, 2, 5);
    vec3 lookAt          = vec3(0, 0, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(1.0f, 0, 0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

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
          sphere.transformation.translate = center;
          world.Add(make_shared<Sphere>(sphere));
        }
      }
    }
    auto mat1                        = make_shared<Dielectric>(1.5);
    auto sphere1                     = Sphere(1.0, mat1);
    sphere1.transformation.translate = vec3(0, 1, 0);
    world.Add(make_shared<Sphere>(sphere1));

    auto mat2                        = make_shared<Lambertian>(vec3(0.4, 0.2, 0.1));
    auto sphere2                     = Sphere(1.0, mat2);
    sphere2.transformation.translate = vec3(-4, 1, 0);
    world.Add(make_shared<Sphere>(sphere2));

    auto mat3                        = make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
    auto sphere3                     = Sphere(1.0, mat3);
    sphere3.transformation.translate = vec3(4, 1, 0);
    world.Add(make_shared<Sphere>(sphere3));

    auto groundMaterial                   = make_shared<Lambertian>(vec3(0.5f));
    auto groundSphere                     = Sphere(1000, make_shared<Lambertian>(groundMaterial));
    groundSphere.transformation.translate = vec3(0, -1000, 0);
    world.Add(make_shared<Sphere>(groundSphere));

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::RandomMovingSpheres(int imageWidth, int imageHeight, int ballGridWidth, int ballGridHeight) {
    Scene        s;
    HittableList world;

    vec3 lookFrom        = vec3(13, 2, 3);
    vec3 lookAt          = vec3(0, 0, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(1.0f, 0, 0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus, 0.0, 1.0);

    auto checker = std::make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));

    auto groundMaterial                   = make_shared<Lambertian>(vec3(0.5f));
    auto groundSphere                     = Sphere(1000, make_shared<Lambertian>(checker));
    groundSphere.transformation.translate = vec3(0, -1000, 0);
    world.Add(make_shared<Sphere>(groundSphere));

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
            world.Add(make_shared<MovingSphere>(center, center2, 0.0, 1.0, 0.2, sphereMaterial));
          } else if (chooseMat < 0.95) {
            // Metal
            auto  albedo   = vec3::Random(0.5, 1);
            float fuzz     = RandomFloat(0, 0.5);
            sphereMaterial = make_shared<Metal>(albedo, fuzz);

            auto sphere                     = Sphere(0.2, sphereMaterial);
            sphere.transformation.translate = center;
            world.Add(make_shared<Sphere>(sphere));

          } else {
            sphereMaterial                  = make_shared<Dielectric>(1.5);
            auto sphere                     = Sphere(0.2, sphereMaterial);
            sphere.transformation.translate = center;
            world.Add(make_shared<Sphere>(sphere));
          }
        }
      }
    }

    auto mat1                        = make_shared<Dielectric>(1.5);
    auto sphere1                     = Sphere(1.0, mat1);
    sphere1.transformation.translate = vec3(0, 1, 0);

    world.Add(make_shared<Sphere>(sphere1));

    auto mat2                        = make_shared<Lambertian>(vec3(0.4, 0.2, 0.1));
    auto sphere2                     = Sphere(1.0, mat2);
    sphere2.transformation.translate = vec3(-4, 1, 0);

    world.Add(make_shared<Sphere>(sphere2));

    auto mat3                        = make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
    auto sphere3                     = Sphere(1.0, mat3);
    sphere3.transformation.translate = vec3(4, 1, 0);

    world.Add(make_shared<Sphere>(sphere3));

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::TwoSpheres(int imageWidth, int imageHeight) {
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    Camera cam(
        vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0), vec3::Zero(), 20.0, (float)imageWidth / imageHeight, 0.0, 10, 0, 0
    );

    Scene s;

    // auto checker =
    //     make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9,
    //     0.9));

    auto           perlinTexture    = make_shared<NoiseTexture>(4.0f);
    sPtr<Material> lambertianPerlin = make_shared<Lambertian>(perlinTexture);

    HittableList world;

    // No need to use a BVH since there are only two spheres

    auto sphere1 = Sphere(10, lambertianPerlin);
    sphere1.changeMaterial(lambertianPerlin);
    sphere1.transformation.translate = vec3(0, -10, 0);

    auto sphere2 = Sphere(10, lambertianPerlin);
    sphere2.changeMaterial(lambertianPerlin);
    sphere2.transformation.translate = vec3(0, 10, 0);

    world.Add(make_shared<Sphere>(sphere1));
    world.Add(make_shared<Sphere>(sphere2));

    s = Scene(new HittableList(world), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::Earth(int imageWidth, int imageHeight) {
    Scene        s;
    HittableList world;

    vec3 lookFrom        = vec3(13, 2, 3);
    vec3 lookAt          = vec3(0, 0, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(0.0f, 0, 0.0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus, 0.0, 1.0);

    auto earthTexture = make_shared<ImageTexture>("assets/textures/earthmap.png");
    auto earthSurface = make_shared<Lambertian>(earthTexture);
    // auto earthSurface = make_shared<Metal>(vec3(0.7,0.2,0.7), 0.7);
    auto globe = make_shared<Sphere>(2, earthSurface);
    world.Add(globe);

    s = Scene(new HittableList(world), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::Light(int imageWidth, int imageHeight) {
    Scene        s;
    HittableList world;

    vec3 lookFrom        = vec3(26, 3, 6);
    vec3 lookAt          = vec3(0, 2, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3::Zero();

    float distToFocus = 10.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(0.0f, 0, 0.0);
    int   fov         = 20.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus, 0.0, 1.0);

    auto perText   = make_shared<NoiseTexture>(4.0f);
    auto diffLight = make_shared<DiffuseLight>(vec3(4, 4, 4));

    auto sphere1                     = Sphere(1000, make_shared<Lambertian>(perText));
    sphere1.transformation.translate = vec3(0, -1000, 0);
    world.Add(make_shared<Sphere>(sphere1));

    auto sphere2                     = Sphere(2, make_shared<Lambertian>(perText));
    sphere2.transformation.translate = vec3(0, 2, 0);
    world.Add(make_shared<Sphere>(sphere2));

    world.Add(make_shared<XYRect>(3, 5, 1, 3, -2, diffLight));

    s = Scene(new HittableList(world), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::CornellBox(int imageWidth, int imageHeight) {

    Scene         s;
    HittableList *world = new HittableList();

    vec3 lookFrom        = vec3(27.8, 27.8, -80);
    vec3 lookAt          = vec3(27.8, 27.8, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3(0.2, 0.2, 0.2);

    float distToFocus = 80.0f;
    float aperature   = 0.1f;
    vec3  moveDir     = vec3(0.0f, 0, 0.0);
    int   fov         = 40.0;

    Camera cam(
        lookFrom, lookAt, vUp, moveDir, fov, float(imageWidth) / float(imageHeight), aperature, distToFocus, 0.0, 1.0
    );
    cam.angle.y = 3.14; // Rotate towards the scene

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

    // Left wall
    world->Add(HittableBuilder<Plane>(wallHeight, wallHeight)
                   .withTranslation(vec3(wallWidth, wallWidth / 2, wallHeight / 2))
                   .withRotation(vec3(0, 0, 90))
                   .withMaterial(green)
                   .withName("left wall")
                   .build());

    // Right wall
    world->Add(HittableBuilder<Plane>(wallWidth, wallHeight)
                   .withTranslation(vec3(0, wallWidth / 2, wallHeight / 2))
                   .withRotation(vec3(0, 0, -90))
                   .withMaterial(red)
                   .withName("right wall")
                   .build());
    // Top light
    world->Add(HittableBuilder<Plane>(wallWidth, wallHeight)
                   .withTranslation(vec3(wallWidth / 2, wallHeight, wallHeight / 2))
                   .withRotation(vec3(0, 0, 180))
                   .withMaterial(light)
                   .withName("light")
                   .build());
    // floor
    world->Add(HittableBuilder<Plane>(wallWidth, wallHeight)
                   .withTranslation(vec3(wallWidth / 2, 0, wallHeight / 2))
                   .withMaterial(chrome)
                   .withName("floor")
                   .build());

    // Back wall
    world->Add(HittableBuilder<Plane>(wallWidth, wallHeight)
                   .withTranslation(vec3(wallWidth / 2, wallHeight / 2, wallWidth))
                   .withRotation(vec3(0, 90, -90))
                   .withMaterial(white)
                   .withName("back wall")
                   .build());

    world->Add(HittableBuilder<Box>(vec3(16.5, 33, 16.5))
                   .withTranslation(vec3(40, 16.5, 40))
                   .withRotation(vec3(0, 15, 0))
                   .withMaterial(white)
                   .withName("Box 1")
                   .build());

    world->Add(HittableBuilder<Box>(vec3(10))
                   .withTranslation(vec3(10, 5, 10))
                   .withRotation(vec3(0, 12, 0))
                   .withMaterial(purplishMetal)
                   .withName("Box 2")
                   .build());

    s = Scene(new BVHNode(world->objects, 0, 1), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::Load(int imageWidth, int imageHeight, std::string path) {
    std::ifstream jsonFile(path);
    std::cout << "Loading scene from file " << path << std::endl;

    json readScene;
    jsonFile >> readScene;

    json  settings    = readScene["settings"];
    float aspectRatio = (float)imageWidth / imageHeight;
    Scene s           = Scene(
        nullptr,
        Camera(readScene["camera"], aspectRatio),
        settings["max_depth"].get<int>(),
        imageWidth,
        imageHeight,
        settings["num_samples"].get<int>(),
        settings["background_color"].get<vec3>()
    );

    auto world = HittableList();

    std::cout << std::setw(4) << readScene["objects"] << std::endl;
    for (const auto &obj : readScene["objects"]) {

      auto objPtr = ObjectFactory::FromJson(obj);
      if (objPtr)
        world.Add(objPtr);
    }

    std::cout << "Loaded scene with\n"
              << "\tsettings " << settings << std::endl
              << "\t#objects " << world.objects.size() << std::endl;

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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    // auto materialGround = make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
    // auto materialCenter = make_shared<Lambertian>(vec3(0.1, 0.2, 0.5));

    std::string skyspherePath = "assets/textures/skysphere.png";
    auto        sphereMat     = make_shared<DiffuseLight>(make_shared<ImageTexture>(skyspherePath.c_str(), true));

    // auto light          = make_shared<DiffuseLight>(make_shared<SolidColor>(vec3(3)));
    auto metal = make_shared<Metal>(vec3(0.8), 0.1f);

    auto box    = HittableBuilder<Box>(1).withMaterial(metal).build();
    auto sphere = make_shared<Sphere>(0.5f, sphereMat);

    box->transformation = Transformation(vec3(0, 0, -5), vec3(0, 0, 0));
    box->name           = "Cube";
    sphere->name        = "Sphere";

    std::vector<sPtr<Hittable>> world = {box, sphere};

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);
    s.addSkysphere(skyspherePath);

    return s;
  }

  Scene Scene::PlaneTest(int imageWidth, int imageHeight) {
    Scene        s;
    HittableList world;
    vec3         lookFrom        = vec3(0, 3, 0);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0.1);
    vec3         backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(0, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto red = make_shared<DiffuseLight>(vec3(3, 0, 0));
    auto magenta =
        make_shared<DiffuseLight>(make_shared<CheckerTexture>(CheckerTexture(vec3(3, 0, 3), vec3::Zero(), 40)));
    auto blue = make_shared<DiffuseLight>(make_shared<CheckerTexture>(CheckerTexture(vec3(0, 0, 3), vec3::Zero(), 40)));

    auto earth = make_shared<DiffuseLight>(make_shared<ImageTexture>("assets/textures/earthmap.png"));

    auto plane = HittableBuilder<Plane>(3, 3).withMaterial(magenta).withTranslation(vec3(-3, 10, 0)).build();

    auto xzplane = make_shared<XZRect>(-1.5, 1.5, -1.5, 1.5, 0, blue);
    auto tri     = HittableBuilder<Triangle>(vec3(0, 0, 1), vec3(-1, 0, -1), vec3(1, 0, 0)).withMaterial(red).build();

    std::vector<sPtr<Hittable>> obj = {plane, xzplane, tri};

    world //
        .Add(tri)
        .Add(plane)
        .Add(xzplane);

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);
    // s.objects.objects = obj;

    return s;
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto red = make_shared<DiffuseLight>(vec3(3, 0, 0));

    auto box = HittableBuilder<Box>(vec3(1, 1, 1), vec3(2, 2, 2)).withMaterial(red).build();

    s = Scene(new HittableList(box), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  json Scene::GetObjArray() const {

    auto              objectList = worldRoot->getChildrenAsList();
    std::vector<json> objJsons;

    for (const auto &obj : objectList) {
      json temp = obj->toJson();
      std::cout << temp << std::endl;
      objJsons.push_back(temp);
    }

    return objJsons;
  }

  json Scene::toJson() const {
    json sceneJson;
    to_json(sceneJson, *this);
    return sceneJson;
  }
} // namespace rt