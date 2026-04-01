#pragma once

#include "data_structures/vec3.h"
#include "common/Ray.h"

#include <glm/ext/matrix_float4x4.hpp>

#include <optional>

namespace rt
{
  class Ray;
  class Transformation;

  struct LocalspaceRaycastResult {
    vec3 hitpoint;
    vec3 normal;
    float t;
  };

  struct WorldspaceRaycastResult {
    vec3 hitpoint;
    vec3 normal;
    float t;
  };

  template <typename Transformation>
  auto rayWorldToLocal(const rt::Ray &worldspaceRay, const Transformation &transformation) -> rt::Ray {
    return rt::Ray{
        transformation.Inverse(worldspaceRay.origin),
        transformation.ApplyInverseRotation(worldspaceRay.direction),
        worldspaceRay.time,
    };
  }

  // Because we have rt::Transformation and rt::editor::Transformation
  template <typename Transformation>
  auto raycastInLocalspace(
      const rt::Ray &worldspaceRay, const Transformation &transformation,
      std::function<std::optional<LocalspaceRaycastResult>(const rt::Ray &localspaceRay)> localspaceRaycastFn)
      -> std::optional<WorldspaceRaycastResult> {
    auto const localspaceRay = rayWorldToLocal(worldspaceRay, transformation);

    return localspaceRaycastFn(localspaceRay).transform([&](auto const &localspaceRaycastResult) {
      auto const worldspaceHitPoint = transformation.Apply(localspaceRaycastResult.hitpoint);
      auto const worldspaceNormal   = transformation.ApplyRotation(localspaceRaycastResult.normal).Normalize();

      return WorldspaceRaycastResult{worldspaceHitPoint, worldspaceNormal};
    });
  }

  struct SphereRaycastResult {
    float t; // Distance along the ray
  };

  // In localspace, the sphere is centered at the origin.
  std::optional<SphereRaycastResult> raycastSphere(const rt::Ray &localspaceRay, float radius, float tMin,
                                                   float tMax);

  struct BoxRaycastResult {
    float t; // Distance along the ray
    vec3  normal;
  };
  std::optional<BoxRaycastResult> raycastBox(const rt::Ray &localspaceRay, vec3 extents, float tMin, float tMax);

  struct PlaneRaycastResult {
    float t;
  };
  std::optional<PlaneRaycastResult> raycastPlane(const rt::Ray &localspaceRay, float width, float height, float tMin,
                                                 float tMax);
}
