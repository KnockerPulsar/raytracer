#include "Scene.h"
#include "AABB.h"
#include "BVHNode.h"
#include "Defs.h"
#include "Transformation.h"

#include "Camera.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"
#include "Util.h"
#include "data_structures/vec3.h"

#include "materials/Dielectric.h"
#include "materials/DiffuseLight.h"
#include "materials/Lambertian.h"
#include "materials/Material.h"
#include "materials/MaterialBuilder.h"

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

using nlohmann::json, std::vector, std::pair, std::string, std::function;

namespace rt {

  vector<pair<string, function<Scene()>>> Scene::builtInScenes = {
      {"Default", Scene::Default},
      {"Scene1", Scene::Scene1},
      {"Scene2", Scene::Scene2},
      {"Random", std::bind(Scene::Random, 10, 10)},
      {"Random Moving", std::bind(Scene::RandomMovingSpheres, 10, 10)},
      {"TwoSpheres", Scene::TwoSpheres},
      {"Earth", Scene::Earth},
      {"Light", Scene::Light},
      {"Cornell", Scene::CornellBox},
      {"Transformation test", Scene::TransformationTest},
      {"Plane test", Scene::PlaneTest},
      {"Raster test", Scene::RasterTest}};

  void Scene::addSkysphere(std::string ssTex) {

    skysphereTexture = ssTex;

    auto tex = std::make_shared<ImageTexture>(ssTex.c_str(), false, true);

    auto skysphereMat = MaterialBuilder<DiffuseLight>().setTexture(tex).build();
    skysphere         = std::make_shared<Sphere>(500.0f, skysphereMat);

    ::Image img = LoadImage(skysphereTexture.c_str());
    ImageFlipVertical(&img);

    skysphereModel = LoadModelFromMesh(EditorUtils::generateSkysphere(500, {32, 32}));
    skysphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTextureFromImage(img);
  }

  void Scene::drawSkysphere() {
    rlDisableBackfaceCulling();
    rlDisableDepthMask();

    DrawModel(skysphereModel, skysphere->transformation.getTranslation().toRlVec3(), 1.0f, WHITE);

    rlEnableDepthMask();
    rlEnableBackfaceCulling();
  }

  Scene Scene::Default() {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus);

    auto mat   = MaterialBuilder<Lambertian>().setTexture(vec3(3, 3, 3)).build();
    auto light = MaterialBuilder<DiffuseLight>().setTexture(vec3(3, 3, 3)).build();

    world //
        .Add(HittableBuilder<Box>(1).withMaterial(mat).build())
        .Add(HittableBuilder<Plane>::CreateHittable(2.0, 2.0)
                 .withTranslation(vec3(2, 0, 2))
                 .withRotation(vec3(0, -45, 90))
                 .withName("light plane")
                 .withMaterial(light)
                 .build());

    s = Scene(new BVHNode(world, 0, 1), cam, backgroundColor);

    return s;
  }

  Scene Scene::Scene1() {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus);

    auto materialGround = MaterialBuilder<Lambertian>().setTexture(vec3(0.8, 0.8, 0.0)).build();
    auto materialCenter = MaterialBuilder<Lambertian>().setTexture(vec3(0.1, 0.2, 0.5)).build();
    auto materialLeft   = MaterialBuilder<Dielectric>().setTexture(vec3(1.5)).build();
    auto materialRight  = MaterialBuilder<Metal>(0.1f).setTexture(vec3(0.8, 0.6, 0.2)).build();

    auto groundSphere = HittableBuilder<Sphere>(100.0)
                            .withMaterial(materialGround)
                            .withTranslation(vec3(0, -100.5, -1))
                            .withName("Ground Sphere")
                            .build();

    auto centerSphere = HittableBuilder<Sphere>(0.5)
                            .withMaterial(materialCenter)
                            .withTranslation(vec3(0, 0, -1.0f))
                            .withName("Center Sphere")
                            .build();

    auto leftSphereOuter = HittableBuilder<Sphere>(0.5)
                               .withMaterial(materialLeft)
                               .withTranslation(vec3(-1, 0, -1))
                               .withName("Left Sphere Outer")
                               .build();

    auto leftSphereInner = HittableBuilder<Sphere>(-0.45)
                               .withMaterial(materialLeft)
                               .withTranslation(vec3(-1, 0, -1))
                               .withName("Left Sphere Inner")
                               .build();

    auto rightSphere = HittableBuilder<Sphere>(0.5)
                           .withMaterial(materialRight)
                           .withTranslation(vec3(1, 0, -1))
                           .withName("Right Sphere")
                           .build();

    world //
        .Add(groundSphere)
        .Add(centerSphere)
        // Th "hollow sphere"
        .Add(leftSphereOuter)
        .Add(leftSphereInner)
        .Add(rightSphere);

    s = Scene(new BVHNode(world, 0, 1), cam, backgroundColor);

    return s;
  } // namespace rt

  Scene Scene::Scene2() {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus);

    auto materialLeft  = MaterialBuilder<Lambertian>().setTexture(vec3(0, 0, 1)).build();
    auto materialRight = MaterialBuilder<Lambertian>().setTexture(vec3(1, 0, 0)).build();

    auto leftSphere = HittableBuilder<Sphere>(r).withMaterial(materialLeft).withTranslation(vec3(-r, 0, -1)).build();

    auto rightSphere = HittableBuilder<Sphere>(r).withMaterial(materialRight).withTranslation(vec3(r, 0, -1)).build();

    world // Scene::Scene
        .Add(leftSphere)
        .Add(rightSphere);

    s = Scene(new BVHNode(world, 0, 1), cam, backgroundColor);

    return s;
  }

  Scene Scene::Random(int ballGridWidth, int ballGridHeight) {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus);

    for (int a = -ballGridWidth; a < ballGridWidth; a++) {
      for (int b = -ballGridHeight; b < ballGridHeight; b++) {
        float chooseMat = RandomFloat();
        vec3  center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());
        if ((center - vec3(4, 0.2, 0)).Len() > 0.9) {
          shared_ptr<Material> sphereMaterial;

          if (chooseMat < 0.8) {
            // Diffuse
            auto albedo    = vec3::Random() * vec3::Random();
            sphereMaterial = MaterialBuilder<Lambertian>().setTexture(albedo).build();
          } else if (chooseMat < 0.95) {
            // Metal
            auto  albedo   = vec3::Random(0.5, 1);
            float fuzz     = RandomFloat(0, 0.5);
            sphereMaterial = MaterialBuilder<Metal>(fuzz).setTexture(albedo).build();
          } else {
            sphereMaterial =
                MaterialBuilder<DiffuseLight>().setTexture(make_shared<NoiseTexture>(1, 5, vec3(1))).build();
          }

          world.Add(HittableBuilder<Sphere>(0.2).withMaterial(sphereMaterial).withTranslation(center).build());
        }
      }
    }
    world.Add(HittableBuilder<Sphere>(1.0f)
                  .withMaterial(MaterialBuilder<Dielectric>(1.5f).build())
                  .withTranslation(vec3(0, 1, 0))
                  .build());

    world.Add(HittableBuilder<Sphere>(1.0)
                  .withMaterial(MaterialBuilder<Lambertian>().setTexture(vec3(0.4, 0.2, 0.1)).build())
                  .withTranslation(vec3(-4, 1, 0))
                  .build());

    world.Add(HittableBuilder<Sphere>(1.0)
                  .withMaterial(MaterialBuilder<Metal>(0.0f).setTexture(vec3(0.7, 0.6, 0.5)).build())
                  .withTranslation(vec3(4, 1, 0))
                  .build());

    world.Add(HittableBuilder<Sphere>(1000)
                  .withMaterial(MaterialBuilder<Lambertian>().setTexture(vec3(0.5f)).build())
                  .withTranslation(vec3(0, -1000, 0))
                  .build());

    s = Scene(new BVHNode(world, 0, 1), cam, backgroundColor);

    return s;
  }

  Scene Scene::RandomMovingSpheres(int ballGridWidth, int ballGridHeight) {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus, 0.0, 1.0);

    auto checker = std::make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));

    auto groundMaterial = MaterialBuilder<Lambertian>().setTexture(vec3(0.5f));

    auto groundSphere = HittableBuilder<Sphere>(1000)
                            .withMaterial(MaterialBuilder<Lambertian>().setTexture(checker).build())
                            .withTranslation(vec3(0, -1000, 0))
                            .build();

    for (int a = -ballGridWidth; a < ballGridWidth; a++) {
      for (int b = -ballGridHeight; b < ballGridHeight; b++) {
        float chooseMat = RandomFloat();
        vec3  center(a + 0.9 * RandomFloat(), 0.2, b + 0.9 * RandomFloat());
        if ((center - vec3(4, 0.2, 0)).Len() > 0.9) {
          sPtr<Material> sphereMaterial;

          if (chooseMat < 0.8) {
            // Diffuse
            auto albedo    = vec3::Random() * vec3::Random();
            sphereMaterial = MaterialBuilder<Lambertian>().setTexture(albedo).build();
            vec3 center2   = center + vec3(0, RandomFloat(0, .5), 0);
            world.Add(make_shared<MovingSphere>(center, center2, 0.0, 1.0, 0.2, sphereMaterial));
          } else if (chooseMat < 0.95) {
            // Metal
            auto  albedo   = vec3::Random(0.5, 1);
            float fuzz     = RandomFloat(0, 0.5);
            sphereMaterial = MaterialBuilder<Metal>(fuzz).setTexture(albedo).build();
            auto sphere    = HittableBuilder<Sphere>(0.2).withMaterial(sphereMaterial).withTranslation(center).build();

            world.Add(sphere);

          } else {
            sphereMaterial = MaterialBuilder<Dielectric>(1.5f).build();
            auto sphere    = HittableBuilder<Sphere>(0.2).withMaterial(sphereMaterial).withTranslation(center).build();
            world.Add(sphere);
          }
        }
      }
    }

    auto mat1    = MaterialBuilder<Dielectric>(1.5).build();
    auto sphere1 = HittableBuilder<Sphere>(1.0).withMaterial(mat1).withTranslation(vec3(0, 1, 0)).build();
    world.Add(sphere1);

    auto mat2    = MaterialBuilder<Lambertian>().setTexture(vec3(0.4, 0.2, 0.1)).build();
    auto sphere2 = HittableBuilder<Sphere>(1.0).withMaterial(mat2).withTranslation(vec3(-4, 1, 0)).build();
    world.Add(sphere2);

    auto mat3    = MaterialBuilder<Metal>(0.0).setTexture(vec3(0.7, 0.6, 0.5)).build();
    auto sphere3 = HittableBuilder<Sphere>(1.0).withMaterial(mat3).withTranslation(vec3(4, 1, 0)).build();
    world.Add(sphere3);

    s = Scene(new BVHNode(world, 0, 1), cam, backgroundColor);

    return s;
  }

  Scene Scene::TwoSpheres() {
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    Camera cam(
        vec3(13, 2, 3), vec3(0, 0, 0), vec3(0, 1, 0), vec3::Zero(), 20.0, 0.0, 10, 0, 0
    );

    Scene s;

    // auto checker =
    //     make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9,
    //     0.9));

    auto perlinTexture  = make_shared<NoiseTexture>(4.0f);
    auto checkerTexture = make_shared<CheckerTexture>(vec3(0), vec3(1), 6);

    sPtr<Material> lambertianPerlin  = MaterialBuilder<Lambertian>().setTexture(perlinTexture).build();
    sPtr<Material> lambertianChecker = MaterialBuilder<Lambertian>().setTexture(checkerTexture).build();

    HittableList world;

    // No need to use a BVH since there are only two spheres
    world.Add(HittableBuilder<Sphere>(10).withTranslation(vec3(0, -10, 0)).withMaterial(lambertianPerlin).build());
    world.Add(HittableBuilder<Sphere>(10).withTranslation(vec3(0, 10, 0)).withMaterial(lambertianChecker).build());

    s = Scene(new HittableList(world), cam, backgroundColor);

    return s;
  }

  Scene Scene::Earth() {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus, 0.0, 1.0);

    auto earthTexture = make_shared<ImageTexture>("assets/textures/earthmap.png");
    auto earthSurface = MaterialBuilder<Lambertian>().setTexture(earthTexture).build();
    // auto earthSurface = make_shared<Metal>(vec3(0.7,0.2,0.7), 0.7);
    auto globe = make_shared<Sphere>(2, earthSurface);
    world.Add(globe);

    s = Scene(new HittableList(world), cam, backgroundColor);

    return s;
  }

  Scene Scene::Light() {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus, 0.0, 1.0);

    auto perText   = make_shared<NoiseTexture>(4.0f);
    auto diffLight = MaterialBuilder<DiffuseLight>().setTexture(vec3(4, 4, 4)).build();

    world
        .Add(HittableBuilder<Plane>(100, 100)
                 .withMaterial(MaterialBuilder<Lambertian>().setTexture(perText).build())
                 .withName("Plane")
                 .build())
        .Add(HittableBuilder<Sphere>(2)
                 .withTranslation(vec3(0, 2, 0))
                 .withMaterial(MaterialBuilder<Lambertian>().setTexture(perText).build())
                 .withName("Sphere")
                 .build())
        .Add(HittableBuilder<Plane>(2, 2)
                 .withMaterial(diffLight)
                 .withRotation(vec3(90, 0, 0))
                 .withTranslation(vec3(4, 2, -2))
                 .withName("Light")
                 .build());

    s = Scene(new HittableList(world), cam, backgroundColor);

    return s;
  }

  Scene Scene::CornellBox() {

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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus, 0.0, 1.0);
    cam.angle.y = 3.14; // Rotate towards the scene

    auto red           = MaterialBuilder<Lambertian>().setTexture(vec3(0.65, 0.05, 0.05)).build();
    auto white         = MaterialBuilder<Lambertian>().setTexture(vec3(0.73, 0.73, 0.73)).build();
    auto green         = MaterialBuilder<Lambertian>().setTexture(vec3(0.12, 0.45, 0.15)).build();
    auto light         = MaterialBuilder<DiffuseLight>().setTexture(vec3(2, 2, 2)).build();
    auto purplishMetal = MaterialBuilder<Metal>(0.7f).setTexture(vec3(0.8, 0.1, 0.8)).build();
    auto chrome        = MaterialBuilder<Metal>(0.05f).setTexture(vec3(0.8, 0.8, 0.8)).build();
    auto dielectric    = MaterialBuilder<Dielectric>().setTexture(1.5f).build();
    auto noise         = MaterialBuilder<Lambertian>().setTexture(make_shared<NoiseTexture>(0.1f)).build();
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

    s = Scene(new BVHNode(world->objects, 0, 1), cam, backgroundColor);

    return s;
  }

  Scene Scene::Load(std::string path) {
    std::ifstream jsonFile(path);
    std::cout << "Loading scene from file " << path << std::endl;

    json readScene;
    jsonFile >> readScene;

    json  settings    = readScene["settings"];
    Scene s           = Scene(
        nullptr,
        Camera(readScene["camera"]),
        settings["max_depth"].get<int>(),
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

  Scene Scene::TransformationTest() {

    Scene s;
    vec3  lookFrom        = vec3(0, 0, 0);
    vec3  lookAt          = vec3(0, 0, -1);
    vec3  vUp             = vec3(0, 1, 0);
    vec3  backgroundColor = vec3(0.0, 0.0, 0.0);

    float distToFocus = 10.0f;
    float aperature   = 0.0;
    vec3  moveDir     = vec3(0.0f, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus);

    // auto materialGround = make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
    // auto materialCenter = make_shared<Lambertian>(vec3(0.1, 0.2, 0.5));

    std::string skyspherePath = "assets/textures/skysphere.png";
    auto        sphereMat =
        MaterialBuilder<DiffuseLight>().setTexture(make_shared<ImageTexture>(skyspherePath.c_str(), true)).build();

    // auto light          = make_shared<DiffuseLight>(make_shared<SolidColor>(vec3(3)));
    auto metal = MaterialBuilder<Metal>(0.1f).setTexture(vec3(0.8)).build();

    auto box    = HittableBuilder<Box>(1).withMaterial(metal).build();
    auto sphere = make_shared<Sphere>(0.5f, sphereMat);

    box->transformation = Transformation(vec3(0, 0, -5), vec3(0, 0, 0));
    box->name           = "Cube";
    sphere->name        = "Sphere";

    std::vector<sPtr<Hittable>> world = {box, sphere};

    s = Scene(new BVHNode(world, 0, 1), cam, backgroundColor);
    s.addSkysphere(skyspherePath);

    return s;
  }

  Scene Scene::PlaneTest() {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus);

    auto red     = MaterialBuilder<DiffuseLight>().setTexture(vec3(3, 0, 0)).build();
    auto magenta = MaterialBuilder<DiffuseLight>()
                       .setTexture(make_shared<CheckerTexture>(CheckerTexture(vec3(3, 0, 3), vec3::Zero(), 40)))
                       .build();
    auto blue = MaterialBuilder<DiffuseLight>()
                    .setTexture(make_shared<CheckerTexture>(CheckerTexture(vec3(0, 0, 3), vec3::Zero(), 40)))
                    .build();

    auto earth =
        MaterialBuilder<DiffuseLight>().setTexture(make_shared<ImageTexture>("assets/textures/earthmap.png")).build();

    auto plane = HittableBuilder<Plane>(3, 3).withMaterial(magenta).withTranslation(vec3(-3, 10, 0)).build();

    auto xzplane = make_shared<XZRect>(-1.5, 1.5, -1.5, 1.5, 0, blue);
    auto tri     = HittableBuilder<Triangle>(vec3(0, 0, 1), vec3(-1, 0, -1), vec3(1, 0, 0)).withMaterial(red).build();

    std::vector<sPtr<Hittable>> obj = {plane, xzplane, tri};

    world //
        .Add(tri)
        .Add(plane)
        .Add(xzplane);

    s = Scene(new BVHNode(world, 0, 1), cam, backgroundColor);
    // s.objects.objects = obj;

    return s;
  } // namespace rt

  Scene Scene::RasterTest() {
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

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, aperature, distToFocus);

    auto red = MaterialBuilder<DiffuseLight>().setTexture(vec3(3, 0, 0)).build();

    auto box = HittableBuilder<Box>(vec3(1, 1, 1), vec3(2, 2, 2)).withMaterial(red).build();

    s = Scene(new HittableList(box), cam, backgroundColor);

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