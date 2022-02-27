#pragma once
#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"
#include "Util.h"
#include "Vec3.h"
#include <bits/types/FILE.h>
#include <memory>
#include <raylib.h>

namespace raytracer {
  HittableList CreateScene1() {
    HittableList world;

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

    return world;
  }

  HittableList CreateScene2() {

    float        R = cos(pi / 4);
    HittableList world;

    auto material_left  = make_shared<Lambertian>(Vec3(0, 0, 1));
    auto material_right = make_shared<Lambertian>(Vec3(1, 0, 0));

    world //
        .Add(make_shared<Sphere>(R, Vec3(-R, 0, -1), material_left))
        .Add(make_shared<Sphere>(R, Vec3(R, 0, -1), material_right));

    return world;
  }

  HittableList RandomScene() {
    HittableList world;
    auto         groundMaterial = make_shared<Lambertian>(Vec3(0.5f));
    world.Add(make_shared<Sphere>(1000, Vec3(0, -1000, 0), groundMaterial));

    for (int a = -11; a < 11; a++) {
      for (int b = -11; b < 11; b++) {
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

    return world;
  }
} // namespace raytracer