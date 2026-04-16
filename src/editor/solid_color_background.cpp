#include "solid_color_background.h"

#include "raytracer/solid_color_background.h"

rt::editor::SolidColorBackground::SolidColorBackground(vec3 backgroundColor) : backgroundColor(backgroundColor) {}

bool rt::editor::SolidColorBackground::OnImgui() {
  // TODO
  return false;
}

void rt::editor::SolidColorBackground::drawBackground(vec3 /*lookFrom*/) const {
  ClearBackground(backgroundColor.toRaylibColor());
}

auto rt::editor::SolidColorBackground::toRaytracerBackground() const -> std::unique_ptr<raytracer::Background> {

  return std::make_unique<raytracer::SolidColorBackground>(backgroundColor);
}
