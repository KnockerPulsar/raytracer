#include "raycast.h"

#include "common/Ray.h"

#include "data_structures/vec3.h"

std::optional<rt::SphereRaycastResult> rt::raycastSphere(const rt::Ray &localspaceRay, float radius, float tMin,
                                                   float tMax) {
  // Spheres are centered at zero by default
  // We transform the ray hitting the object so no need to move it.
  auto const center = vec3::Zero();

  // Vector between ray origin and sphere center
  vec3  oc    = localspaceRay.origin - center;
  float a     = localspaceRay.direction.SqrLen();
  float halfB = vec3::DotProd(oc, localspaceRay.direction);
  float c     = oc.Len() * oc.Len() - radius * radius;

  float discriminant = halfB * halfB - a * c;
  if (discriminant < 0)
    return {};

  float sqrtDisc = sqrt(discriminant);

  float root = (-halfB - sqrtDisc) / a;
  if (root < tMin || tMax < root) {
    root = (-halfB + sqrtDisc) / a;
    if (root < tMin || tMax < root)
      return {};
  }

  return {{root}};
}

std::optional<rt::BoxRaycastResult> rt::raycastBox(const rt::Ray &localspaceRay, vec3 min, vec3 max, float tMin, float tMax) {
  // When raycasting in localspace, if the raycast hits the -x face first, then
  // the normal is {-1, 0, 0}. If it hits the +y face first, then the normal is
  // {1, 0, 0}. This pattern extends to all other faces.

  vec3 outwardNormal{0};
  {
    auto const invD = 1.0f / localspaceRay.direction.x;
    auto t0   = (min.x - localspaceRay.origin.x) * invD;
    auto t1   = (max.x - localspaceRay.origin.x) * invD;
    if (invD < 0.0f)
      std::swap(t0, t1);

    if (t0 > tMin) {
      tMin = t0;

      // invD's sign encodes the direction of the ray in this axis. The outward
      // normal is in the opposite direction;
      auto const sign = invD > 0 ? 1 : -1;
      outwardNormal   = -sign * vec3{1, 0, 0};
    }

    tMin = t0 > tMin ? t0 : tMin;

    tMax = t1 < tMax ? t1 : tMax;
    if (tMax <= tMin)
      return {};
  }

  {
    auto const invD = 1.0f / localspaceRay.direction.y;
    auto t0   = (min.y - localspaceRay.origin.y) * invD;
    auto t1   = (max.y - localspaceRay.origin.y) * invD;
    if (invD < 0.0f)
      std::swap(t0, t1);

    if (t0 > tMin) {
      tMin            = t0;
      auto const sign = invD > 0 ? 1 : -1;
      outwardNormal   = -sign * vec3{0, 1, 0};
    }

    tMax = t1 < tMax ? t1 : tMax;
    if (tMax <= tMin)
      return {};
  }

  {
    auto invD = 1.0f / localspaceRay.direction.z;
    auto t0   = (min.z - localspaceRay.origin.z) * invD;
    auto t1   = (max.z - localspaceRay.origin.z) * invD;
    if (invD < 0.0f)
      std::swap(t0, t1);

    if (t0 > tMin) {
      tMin            = t0;
      auto const sign = invD > 0 ? 1 : -1;
      outwardNormal   = -sign * vec3{0, 0, 1};
    }

    tMax = t1 < tMax ? t1 : tMax;
    if (tMax <= tMin)
      return {};
  }

  return {{tMin, outwardNormal}};
}

std::optional<rt::PlaneRaycastResult> rt::raycastPlane(const rt::Ray &localspaceRay, float width, float height,
                                                       float tMin, float tMax) {
  auto const y = 0; // By default, all planes are XZ planes
  float      t = (y - localspaceRay.origin.y) / localspaceRay.direction.y;

  if (t < tMin || t > tMax)
    return {};

  auto [x, _, z] = localspaceRay.At(t);

  if (x < -width / 2 || x > width / 2 || z < -height / 2 || z > height / 2)
    return {};

  return {{t}};
}
