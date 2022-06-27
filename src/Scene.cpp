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

using nlohmann::json;

namespace rt {

  void Scene::addSkysphere(std::string ssTex) {

    skysphereTexture = ssTex;

    auto tex = std::make_shared<ImageTexture>(ssTex.c_str(), false, true);

    auto skysphereMat = std::make_shared<DiffuseLight>(tex);
    skysphere         = std::make_shared<Sphere>(500.0f, vec3(0), skysphereMat);

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
    vec3         lookFrom        = vec3(-1, 1, 1);
    vec3         lookAt          = vec3(0, 0, 0);
    vec3         vUp             = vec3(0, 1, 0);
    vec3         backgroundColor = vec3(0.70, 0.80, 1.00);

    float distToFocus = 10.0f;
    float aperature   = 0.001F;
    vec3  moveDir     = vec3(10.0f, 0, 0);
    int   fov         = 60.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus);

    auto mat = make_shared<DiffuseLight>(vec3(2.0f, 2.0f, 2.0f));

    world //
        .Add(make_shared<Box>(vec3::Zero(), mat));

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

    world //
        .Add(make_shared<Sphere>(100.0, vec3(0, -100.5, -1), materialGround))
        .Add(make_shared<Sphere>(0.5, vec3(0, 0, -1.0), materialCenter))

        // These 2 spheres make a "hollow sphere"
        .Add(make_shared<Sphere>(0.5, vec3(-1., 0, -1), materialLeft))
        .Add(make_shared<Sphere>(-0.45, vec3(-1., 0, -1), materialLeft))

        .Add(make_shared<Sphere>(0.5, vec3(1., 0, -1), materialRight));

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

    world //
        .Add(make_shared<Sphere>(r, vec3(-r, 0, -1), materialLeft))
        .Add(make_shared<Sphere>(r, vec3(r, 0, -1), materialRight));

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
          world.Add(make_shared<Sphere>(0.2, center, sphereMaterial));
        }
      }
    }
    auto mat1 = make_shared<Dielectric>(1.5);
    world.Add(make_shared<Sphere>(1.0, vec3(0, 1, 0), mat1));

    auto mat2 = make_shared<Lambertian>(vec3(0.4, 0.2, 0.1));
    world.Add(make_shared<Sphere>(1.0, vec3(-4, 1, 0), mat2));

    auto mat3 = make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Sphere>(1.0, vec3(4, 1, 0), mat3));

    auto groundMaterial = make_shared<Lambertian>(vec3(0.5f));
    world.Add(make_shared<Sphere>(1000, vec3(0, -1000, 0), make_shared<Lambertian>(groundMaterial)));

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

    auto groundMaterial = make_shared<Lambertian>(vec3(0.5f));
    world.Add(make_shared<Sphere>(1000, vec3(0, -1000, 0), make_shared<Lambertian>(checker)));

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
            world.Add(make_shared<Sphere>(0.2, center, sphereMaterial));

          } else {
            sphereMaterial = make_shared<Dielectric>(1.5);
            world.Add(make_shared<Sphere>(0.2, center, sphereMaterial));
          }
        }
      }
    }
    auto mat1 = make_shared<Dielectric>(1.5);
    world.Add(make_shared<Sphere>(1.0, vec3(0, 1, 0), mat1));

    auto mat2 = make_shared<Lambertian>(vec3(0.4, 0.2, 0.1));
    world.Add(make_shared<Sphere>(1.0, vec3(-4, 1, 0), mat2));

    auto mat3 = make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Sphere>(1.0, vec3(4, 1, 0), mat3));

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::TwoSpheres(int imageWidth, int imageHeight) {
    vec3 backgroundColor = vec3(0.70, 0.80, 1.00);

    Camera cam(vec3(13, 2, 3),
               vec3(0, 0, 0),
               vec3(0, 1, 0),
               vec3::Zero(),
               20.0,
               (float)imageWidth / imageHeight,
               0.0,
               10,
               0,
               0);

    Scene s;

    // auto checker =
    //     make_shared<CheckerTexture>(vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9,
    //     0.9));

    auto perText = make_shared<NoiseTexture>(4.0f);

    HittableList world;
    // No need to use a BVH since there are only two spheres
    world.Add(make_shared<Sphere>(10, vec3(0, -10, 0), make_shared<Lambertian>(perText)));
    world.Add(make_shared<Sphere>(10, vec3(0, 10, 0), make_shared<Lambertian>(perText)));

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
    auto globe = make_shared<Sphere>(2, vec3(0, 0, 0), earthSurface);
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

    world.Add(make_shared<Sphere>(1000, vec3(0, -1000, 0), make_shared<Lambertian>(perText)));

    world.Add(make_shared<Sphere>(2, vec3(0, 2, 0), make_shared<Lambertian>(perText)));

    world.Add(make_shared<XYRect>(3, 5, 1, 3, -2, diffLight));

    s = Scene(new HittableList(world), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::CornellBox(int imageWidth, int imageHeight) {

    Scene        s;
    HittableList world;

    vec3 lookFrom        = vec3(27.8, 27.8, -80);
    vec3 lookAt          = vec3(27.8, 27.8, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3::Zero();

    float distToFocus = 80.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(0.0f, 0, 0.0);
    int   fov         = 40.0;

    Camera cam(
        lookFrom, lookAt, vUp, moveDir, fov, float(imageWidth) / float(imageHeight), aperature, distToFocus, 0.0, 1.0);
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

    // Left wall
    auto leftWall = make_shared<Plane>(55.5, 55.5, light);
    leftWall->setTransformation(vec3(55.5, leftWall->w / 2, leftWall->h / 2), vec3(0, 0, 90));
    leftWall->name = "leftWall";
    world.Add(leftWall);

    // // Right wall
    // auto rightWall = make_shared<Plane>(vec3(0), 55.5, 55.5, red);
    // rightWall->setTransformation(vec3(0, leftWall->w / 2, leftWall->h / 2), vec3(0, 0, -90));
    // rightWall->name = "rightWall";
    // world.Add(rightWall);

    // // Top light
    // auto topLight = make_shared<Plane>(vec3(0), 55.5, 55.5, light);
    // topLight->setTransformation(vec3(leftWall->w / 2, 55.5, leftWall->h / 2), vec3(0, 0, 180));
    // topLight->name = "topLight";
    // world.Add(topLight);

    // // floor
    // auto floor = make_shared<Plane>(vec3(0), 55.5, 55.5, chrome);
    // floor->setTransformation(vec3(leftWall->w / 2, 0, leftWall->h / 2), vec3(0, 0, 0));
    // floor->name = "floor";
    // world.Add(floor);

    // // ceiling
    // // world.Add(make_shared<XZRect>(0, 555, 0, 555, 555, metal));

    // // Back wall
    // auto backWall = make_shared<Plane>(vec3(0), 55.5, 55.5, white);
    // backWall->setTransformation(vec3(leftWall->w / 2, leftWall->h / 2, 55.5), vec3(0, 90, -90));
    // backWall->name = "backWall";
    // world.Add(backWall);

    // auto b1 = Box(-vec3(16.5, 33, 16.5) / 2, vec3(16.5, 33, 16.5) / 2, white);
    // b1.setTransformation(vec3(40, 16.5, 40), vec3(0, 15, 0));
    // b1.name = "Box1";
    // world.Add(make_shared<Box>(b1));

    // auto b2 = Box(vec3(-5), vec3(5), purplishMetal);
    // b2.setTransformation(vec3(10, 5, 10), vec3(0, 12, 0));
    // b2.name   = "Box2";
    // auto box2 = make_shared<Box>(b2);
    // world.Add(box2);

    s = Scene(new BVHNode(world, 0, 1), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  Scene Scene::Load(int imageWidth, int imageHeight, std::string path) {
    std::ifstream jsonFile(path);
    std::cout << "Loading scene from file " << path << std::endl;

    json readScene;
    jsonFile >> readScene;

    json  settings    = readScene["settings"];
    float aspectRatio = (float)imageWidth / imageHeight;
    Scene s           = Scene(nullptr,
                    Camera(readScene["camera"], aspectRatio),
                    settings["max_depth"].get<int>(),
                    imageWidth,
                    imageHeight,
                    settings["num_samples"].get<int>(),
                    settings["background_color"].get<vec3>());

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

    auto box    = make_shared<Box>(vec3(-0.5, -0.5, -0.5), vec3(0.5, 0.5, 0.5), metal);
    auto sphere = make_shared<Sphere>(0.5f, vec3(0), sphereMat);

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

    auto plane = make_shared<Plane>(3, 3, magenta);
    plane->setTransformation(vec3(-3, 10, 0), vec3(0, 0, 0));

    auto xzplane = make_shared<XZRect>(-1.5, 1.5, -1.5, 1.5, 0, blue);
    auto tri     = make_shared<Triangle>(vec3(0, 0, 1), vec3(-1, 0, -1), vec3(1, 0, 0), red);

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

    auto box = ms<Box>(Box(vec3(1, 1, 1), vec3(2, 2, 2), red));

    s = Scene(new HittableList(box), cam, imageWidth, imageHeight, backgroundColor);

    return s;
  }

  json Scene::GetObjArray() const {

    auto              objectList = worldRoot->getChildrenAsList();
    std::vector<json> objJsons;

    for (const auto &obj : objectList) {
      json temp = obj->GetJson();
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