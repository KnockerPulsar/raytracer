#pragma once
#include "HittableList.h"
#include "Material.h"
#include "Sphere.h"

namespace raytracer {
  HittableList CreateScene1() {
    HittableList world;

    auto material_ground = make_shared<Lambertian>(Vec3(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(Vec3(0.1, 0.2, 0.5));
    auto material_left   = make_shared<Dielectric>(1.5);
    auto material_right  = make_shared<Metal>(Vec3(0.8, 0.6, 0.2), 0.1);

    world //
        .Add(make_shared<Sphere>(100.0, Vec3(0, -100.5, -1), material_ground))
        .Add(make_shared<Sphere>(0.5, Vec3(-0.5, 0, -2.0), material_center))

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
} // namespace raytracer