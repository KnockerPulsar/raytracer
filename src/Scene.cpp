#include "Scene.h"
#include "AABB.h"
#include "BVHNode.h"
#include "Defs.h"
#include "Transformation.h"
#include "materials/DiffuseLight.h"

#include "Camera.h"
#include "ConstantMedium.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Ray.h"
#include "Util.h"
#include "data_structures/vec3.h"
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

  Scene Scene::Scene1(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {
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

    auto material_ground = make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(vec3(0.1, 0.2, 0.5));
    auto material_left   = make_shared<Dielectric>(1.5);
    auto material_right  = make_shared<Metal>(vec3(0.8, 0.6, 0.2), 0.1);

    world //
        .Add(make_shared<Sphere>(100.0, vec3(0, -100.5, -1), material_ground))
        .Add(make_shared<Sphere>(0.5, vec3(0, 0, -1.0), material_center))

        // These 2 spheres make a "hollow sphere"
        .Add(make_shared<Sphere>(0.5, vec3(-1., 0, -1), material_left))
        .Add(make_shared<Sphere>(-0.45, vec3(-1., 0, -1), material_left))

        .Add(make_shared<Sphere>(0.5, vec3(1., 0, -1), material_right));

    s = Scene(new BVHNode(world, 0, 1), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  } // namespace rt

  Scene Scene::Scene2(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {
    Scene        s;
    float        R = cos(pi / 4);
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

    auto material_left  = make_shared<Lambertian>(vec3(0, 0, 1));
    auto material_right = make_shared<Lambertian>(vec3(1, 0, 0));

    world //
        .Add(make_shared<Sphere>(R, vec3(-R, 0, -1), material_left))
        .Add(make_shared<Sphere>(R, vec3(R, 0, -1), material_right));

    s = Scene(new BVHNode(world, 0, 1), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }

  Scene Scene::Random(
      int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel, int ballGridWidth, int ballGridHeight) {
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

    s = Scene(new BVHNode(world, 0, 1), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }

  Scene Scene::RandomMovingSpheres(
      int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel, int ballGridWidth, int ballGridHeight) {
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

    s = Scene(new BVHNode(world, 0, 1), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }

  Scene Scene::TwoSpheres(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {
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

    s = Scene(new HittableList(world), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }

  Scene Scene::Earth(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {
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

    auto earthTexture = make_shared<ImageTexture>("earthmap.png");
    auto earthSurface = make_shared<Lambertian>(earthTexture);
    // auto earthSurface = make_shared<Metal>(vec3(0.7,0.2,0.7), 0.7);
    auto globe = make_shared<Sphere>(2, vec3(0, 0, 0), earthSurface);
    world.Add(globe);

    s = Scene(new HittableList(world), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }

  Scene Scene::Light(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {
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

    s = Scene(new HittableList(world), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }

  Scene Scene::CornellBox(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {

    Scene        s;
    HittableList world;

    vec3 lookFrom        = vec3(278, 278, -800);
    vec3 lookAt          = vec3(278, 278, 0);
    vec3 vUp             = vec3(0, 1, 0);
    vec3 backgroundColor = vec3::Zero();

    float distToFocus = 800.0f;
    float aperature   = 0.1F;
    vec3  moveDir     = vec3(0.0f, 0, 0.0);
    int   fov         = 40.0;

    Camera cam(lookFrom, lookAt, vUp, moveDir, fov, (float)imageWidth / imageHeight, aperature, distToFocus, 0.0, 1.0);

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
    world.Add(make_shared<YZRect>(0, 555, 0, 555, 555, green));

    // Right wall
    world.Add(make_shared<YZRect>(0, 555, 0, 555, 0, red));

    // Top light
    world.Add(make_shared<XZRect>(0, 555, 0, 555, 554, light));

    // floor
    world.Add(make_shared<XZRect>(0, 555, 0, 555, 0, chrome));

    // ceiling
    // world.Add(make_shared<XZRect>(0, 555, 0, 555, 555, metal));

    // Back wall
    world.Add(make_shared<XYRect>(0, 555, 0, 555, 555, white));

    // world.Add(make_shared<Sphere>(100, vec3(330, 330, 165), light));

    auto b1 = Box(vec3(0, 0, 0), vec3(165, 330, 165), white);
    b1.setTransformation(vec3(265, 0, 295), vec3(0, 15, 0));

    world.Add(make_shared<Box>(b1));

    auto b2 = Box(vec3::Zero(), vec3(100, 100, 100), purplishMetal);
    b2.setTransformation(vec3(50, 0, 0), vec3(0, 12, 0));
    auto box2 = make_shared<Box>(b2);

    // auto box2 = make_shared<Translate>(make_shared<Box>(b2), vec3(50, 0, 0));
    world.Add(box2);

    // auto box3 = make_shared<Translate>(
    //     make_shared<RotateY>(
    //         make_shared<Box>(vec3(10), vec3(165-10, 165-10, 165-10),
    //         dielectric), -55),
    //     vec3(200, 0, 65));
    // world.Add(box3);

    s = Scene(new BVHNode(world, 0, 1), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }

  Scene Scene::Load(std::string path) {
    std::ifstream jsonFile(path);
    std::cout << "Loading scene from file " << path << std::endl;

    json readScene;
    jsonFile >> readScene;

    json  settings    = readScene["settings"];
    int   imageWidth  = settings["resolution"]["x"].get<int>();
    int   imageHeight = settings["resolution"]["y"].get<int>();
    float aspectRatio = (float)imageWidth / imageHeight;
    Scene s           = Scene(new HittableList(),
                    Camera(readScene["camera"], aspectRatio),
                    settings["max_depth"].get<int>(),
                    imageWidth,
                    imageHeight,
                    settings["num_samples"].get<int>(),
                    settings["background_color"].get<vec3>());

    auto world = HittableList();
    for (const auto &obj : readScene["objects"]) {
      auto objPtr = ObjectFactory::FromJson(obj);
      if (objPtr)
        world.Add(objPtr);
    }

    std::cout << "Loaded scene with\n"
              << "\tsettings " << settings << std::endl
              << "\t#objects " << world.objects.size() << std::endl;

    // s.objects = world;

    auto bvh    = HittableList();
    s.worldRoot = new BVHNode(world, s.cam.time0, s.cam.time1);

    for (HittableList o : bvh.objects) {
      for (auto oo : o.objects) {
        std::cout << "AA";
      }
    }

    return s;
  }

  Scene Scene::TransformationTest(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {

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

    auto materialGround = make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
    auto materialCenter = make_shared<Lambertian>(vec3(0.1, 0.2, 0.5));
    auto earth          = make_shared<DiffuseLight>(make_shared<ImageTexture>("earthmap.png"));
    auto light          = make_shared<DiffuseLight>(make_shared<SolidColor>(vec3(3)));
    auto metal          = make_shared<Metal>(vec3(0.8), 0.2f);

    auto box    = make_shared<Box>(vec3(-0.5, -0.5, -0.5), vec3(0.5, 0.5, 0.5), materialGround);
    auto sphere = make_shared<Sphere>(0.5f, vec3::Zero(), earth);

    sphere->transformation = Transformation(vec3(-1, -1, -3));
    sphere->name           = "Earth";
    box->transformation    = Transformation(vec3(0, 0, -5), vec3(0, 0, 0));
    box->name              = "Cube";
    // auto rotSphere= make_shared<RotateY>(sphere, 45);

    std::vector<sPtr<Hittable>> world = {box, sphere};
    // HittableList* worldList = new HittableList();
    // worldList->Add(box).Add(sphere);

    s = Scene(new BVHNode(world, 0, 1), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }

  Scene Scene::PlaneTest(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {
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

    auto earth = make_shared<DiffuseLight>(make_shared<ImageTexture>("earthmap.png"));

    auto plane = make_shared<Plane>(vec3(0, 0, 0), 3, 3, magenta);
    plane->setTransformation(vec3(-3, 10, 0), vec3(0, 0, 0));

    auto xzplane = make_shared<XZRect>(-1.5, 1.5, -1.5, 1.5, 0, blue);
    auto tri     = make_shared<Triangle>(vec3(0, 0, 1), vec3(-1, 0, -1), vec3(1, 0, 0), red);

    std::vector<sPtr<Hittable>> obj = {plane, xzplane, tri};

    world //
        .Add(tri)
        .Add(plane)
        .Add(xzplane);

    s = Scene(new BVHNode(world, 0, 1), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);
    // s.objects.objects = obj;

    return s;
  } // namespace rt

  Scene Scene::RasterTest(int imageWidth, int imageHeight, int maxDepth, int samplesPerPixel) {
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

    s = Scene(new HittableList(box), cam, maxDepth, imageWidth, imageHeight, samplesPerPixel, backgroundColor);

    return s;
  }
} // namespace rt