#include "lambertian.h"
#include "editor/textures/solid_color.h"

rt::editor::Lambertian::Lambertian(vec3 albedo) : albedo(std::make_shared<SolidColor>(albedo)) {}
rt::editor::Lambertian::Lambertian(std::shared_ptr<Texture> albedo) : albedo(albedo) {}

void rt::editor::Lambertian::OnImgui() { albedo->OnImgui(); }
