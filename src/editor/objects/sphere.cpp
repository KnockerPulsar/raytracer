#include "sphere.h"

#include "common/Ray.h"
#include "common/Constants.h"
#include "common/raycast.h"

#include "objects/Sphere.h"

#include <imgui.h>

rt::editor::Sphere::Sphere(float radius) : radius(radius) {}

std::optional<float> rt::editor::Sphere::hit(const Ray &worldspaceRay) const {
  return rt::raycastSphereWorldspace(worldspaceRay, radius, getTransformation().getTranslation(), rt::constants::epsilon,
                           rt::constants::infinity)
      .transform([](auto const &r) { return r.t; });
}

void rt::editor::Sphere::RasterizeWithoutTransformation(vec3 color) {
  DrawSphere(Vector3Zero(), radius, color.toRaylibColor());
}

auto rt::editor::Sphere::toRaytracable() const -> std::unique_ptr<rt::Raytracable> {
  return std::make_unique<raytracer::Sphere>(radius, getTransformation().toRaytracerTransformation(),
                                             getMaterial().toRaytracerMaterial());
}

bool rt::editor::Sphere::OnImgui() {
  return ImGui::DragFloat("Radius", &radius, constants::dragFloatVSpeed);
}
