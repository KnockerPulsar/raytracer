#include "plane.h"

#include "common/raycast.h"
#include "common/Constants.h"

#include "objects/Plane.h"

#include <imgui.h>

rt::editor::Plane::Plane(float w, float h) : w(w), h(h) {}

void rt::editor::Plane::RasterizeWithoutTransformation(vec3 color) {
  rlDisableBackfaceCulling();

  DrawPlane(Vector3Zero(), {w, h}, color.toRaylibColor());

  rlEnableBackfaceCulling();
}

std::optional<float> rt::editor::Plane::hit(const Ray &worldspaceRay) const {
  auto const localspaceRay = rt::rayWorldToLocal(worldspaceRay, getTransformation());
  return rt::raycastPlaneLocalspace(localspaceRay, w, h, rt::constants::epsilon, rt::constants::infinity)
      .transform([](auto const &r) { return r.t; });
}

auto rt::editor::Plane::toRaytracable() const -> std::unique_ptr<rt::Raytracable> {
  return std::make_unique<raytracer::Plane>(w, h, getTransformation().toRaytracerTransformation(),
                                            getMaterial().toRaytracerMaterial());
}

bool rt::editor::Plane::OnImgui() {
  return ImGui::DragFloat("Plane Dimensions", (&w), constants::dragFloatVSpeed);
}
