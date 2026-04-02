#include "box.h"

#include "common/raycast.h"
#include "common/Constants.h"

#include "objects/Box.h"

rt::editor::Box::Box(float edgeLength) : Box(vec3(edgeLength)) {}
rt::editor::Box::Box(const vec3& extents) : extents(extents) {}

void rt::editor::Box::RasterizeWithoutTransformation(vec3 color) {
  DrawCubeV(Vector3Zero(), extents, color.toRaylibColor());
}

std::optional<float> rt::editor::Box::hit(const rt::Ray &worldspaceRay) const {
  auto const localspaceRay = rt::rayWorldToLocal(worldspaceRay, getTransformation());
  return rt::raycastBox(localspaceRay, extents, rt::constants::epsilon, rt::constants::infinity)
      .transform([](auto const &r) { return r.t; });
}

auto rt::editor::Box::toRaytracerHittable() const -> std::shared_ptr<rt::Hittable>
{
  return std::make_shared<raytracer::Box>(extents, getTransformation().toRaytracerTransformation(),
                                          getMaterial()->toRaytracerMaterial());
}
