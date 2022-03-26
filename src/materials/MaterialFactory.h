#pragma once
#include "../../vendor/nlohmann-json/json.hpp"
#include "Dielectric.h"
#include "DiffuseLight.h"
#include "Lambertian.h"
#include "Material.h"
#include "Metal.h"
#include <memory>
#include <string>

namespace raytracer {

  class MaterialFactory {
  public:
    static std::shared_ptr<raytracer::Material>
    FromJson(nlohmann::json materialJson) {

      string matType = materialJson["type"].get<string>();

      if (matType == "lambertian")
        return std::make_shared<Lambertian>(materialJson);

      if (matType == "metal")
        return std::make_shared<Metal>(materialJson);

      if (matType == "dielectric")
        return std::make_shared<Dielectric>(materialJson);

      if (matType == "diffuse_light")
        return std::make_shared<DiffuseLight>(materialJson);

      return nullptr;
    }
  };
} // namespace raytracer