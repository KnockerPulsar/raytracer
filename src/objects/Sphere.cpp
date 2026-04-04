#include "Sphere.h"

#include "common/Ray.h"
#include "common/raycast.h"

#include "materials/Material.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <raylib.h>
#include <raymath.h>

namespace rt::raytracer {
  Sphere::Sphere(float r, Transformation const &transformation, sPtr<raytracer::Material> const &material)
      : Raytracable(transformation, material, AABB{vec3{-r}, vec3{r}}), radius(r),
        center(transformation.Apply(vec3{0})) {}

  bool Sphere::Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const {
    if (!worldspaceBoundingBox.Hit(worldspaceRay, t_min, t_max))
      return {};

    auto const raycastResult =
        rt::raycastSphereWorldspace(worldspaceRay, radius, center, t_min, t_max).transform([&](auto const sphereRaycastResult) {
          auto const hitpoint = worldspaceRay.At(sphereRaycastResult.t);
          auto const normal   = (hitpoint - center) / radius;
          return WorldspaceRaycastResult{normal, sphereRaycastResult.t};
        });

    if (!raycastResult)
      return false;

    auto const [worldspaceNormal, t] = *raycastResult;

    rec.p = worldspaceRay.At(t);
    rec.setFaceNormal(worldspaceRay, worldspaceNormal);
    rec.t = t;

    // Convert normal to localspace to get UVs.
    auto const rotatedWorldspaceNormal = transformation.ApplyInverseRotation(worldspaceNormal);
    GetSphereUV(rotatedWorldspaceNormal, rec.u, rec.v);
    rec.mat_ptr    = material;

    return true;
  }

  void Sphere::GetSphereUV(const vec3 &p, float &u, float &v) {
    float theta = std::acos(-p.y);
    float phi   = std::atan2(-p.z, p.x) + PI;

    v = theta / PI;
    u = phi / (2 * PI);
  }
} // namespace rt::raytracer
