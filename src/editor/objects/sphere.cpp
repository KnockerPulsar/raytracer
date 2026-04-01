#include "sphere.h"

#include "common/Ray.h"
#include "common/Constants.h"
#include "common/raycast.h"

rt::editor::Sphere::Sphere(float radius) : radius(radius) {}

std::optional<float> rt::editor::Sphere::hit(const Ray &worldspaceRay) const {
  auto const localspaceRay = rt::rayWorldToLocal(worldspaceRay, getTransformation());
  return rt::raycastSphere(localspaceRay, radius, rt::constants::epsilon, rt::constants::infinity)
      .transform([](auto const &r) { return r.t; });
}

void rt::editor::Sphere::RasterizeWithoutTransformation(vec3 color) {
  DrawSphere(Vector3Zero(), radius, color.toRaylibColor());
}
