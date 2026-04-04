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
    vec3 const normal;
    float const t;
  };

  struct WorldspaceRaycastResult {
    vec3 const normal;
    float const t;
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
      auto const worldspaceNormal = transformation.ApplyRotation(localspaceRaycastResult.normal).Normalize();
      return WorldspaceRaycastResult{worldspaceNormal, localspaceRaycastResult.t};
    });
  }

  struct SphereRaycastResult {
    float t; // Distance along the ray
  };

  std::optional<SphereRaycastResult> raycastSphereWorldspace(const rt::Ray &worldspaceRay, float radius, vec3 center,
                                                             float tMin, float tMax);

  // Yes, this is done in local space. If the AABB has been transformed,
  // inversely transform the ray and use this.
  std::optional<LocalspaceRaycastResult> raycastAABBLocalspace(const rt::Ray &localspaceRay, vec3 min, vec3 max,
                                                               float tMin, float tMax);

  struct PlaneRaycastResult {
    float t;
  };
  // Faster to raycast in localspace.
  std::optional<PlaneRaycastResult> raycastPlaneLocalspace(const rt::Ray &localspaceRay, float width, float height,
                                                           float tMin, float tMax);
}
