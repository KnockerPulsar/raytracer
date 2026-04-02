#include "Sphere.h"

#include "common/Ray.h"
#include "common/raycast.h"

#include "materials/Material.h"

#include <cmath>
#include <memory>
#include <raylib.h>
#include <raymath.h>

namespace rt::raytracer {
  Sphere::Sphere(float r, Transformation const &transformation, sPtr<raytracer::Material> const &material)
      : Hittable(transformation, material, AABB{vec3{-r}, vec3{r}}), radius(r) {}

  bool Sphere::Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const {
    auto const raycastResult = rt::raycastInLocalspace(worldspaceRay, transformation, [&](auto const localspaceRay) {
      return rt::raycastSphere(localspaceRay, radius, t_min, t_max).transform([&](auto const sphereRaycastResult) {
        auto const hitpoint = localspaceRay.At(sphereRaycastResult.t);

        // Spheres are centered at zero by default, we transform the ray instead of the sphere.
        auto const center = vec3::Zero();
        auto const normal = (hitpoint - center) / radius;

        return LocalspaceRaycastResult{hitpoint, normal, sphereRaycastResult.t};
      });
    });

    auto const foo = rt::raycastSphere(Ray{}, radius, t_min, t_max);
    if (!raycastResult)
      return false;

    auto const [worldspaceHitPoint, worldspaceNormal, t] = *raycastResult;

    rec.p = worldspaceHitPoint;
    rec.set_face_normal(worldspaceRay, worldspaceNormal);
    rec.t = t;

    GetSphereUV(worldspaceNormal, rec.u, rec.v);
    rec.mat_ptr    = material;
    rec.closestHit = (Hittable *)this;

    return true;
  }

  void Sphere::GetSphereUV(const vec3 &p, float &u, float &v) {
    float theta = std::acos(-p.y);
    float phi   = std::atan2(-p.z, p.x) + PI;

    v = theta / PI;
    u = phi / (2 * PI);
  }
} // namespace rt
