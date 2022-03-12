#pragma once
#include "Lambertian.h"
#include "Material.h"
#include "Metal.h"
#include <memory>

using raytracer::Lambertian;

class MaterialFactory {
public:
  static std::shared_ptr<raytracer::Material>
  FromJson(nlohmann::json materialJson) {
    if (materialJson["type"].get<string>() == "lambertian")
      return std::make_shared<Lambertian>(materialJson);
    if(materialJson["type"].get<string>() == "metal")
      return std::make_shared<raytracer::Metal>(materialJson);

    return nullptr;
  }
};