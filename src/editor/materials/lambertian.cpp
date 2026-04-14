#include "lambertian.h"
#include "editor/textures/solid_color.h"
#include "materials/Lambertian.h"

rt::editor::Lambertian::Lambertian(vec3 albedo) : albedo(std::make_shared<SolidColor>(albedo)) {}
rt::editor::Lambertian::Lambertian(std::shared_ptr<Texture> albedo) : albedo(albedo) {}

void rt::editor::Lambertian::OnImgui() { albedo->OnImgui(); }

auto rt::editor::Lambertian::toRaytracerMaterial() const -> std::unique_ptr<raytracer::Material> {
  return std::make_unique<raytracer::Lambertian>(albedo->createRaytracerTexture());
}
