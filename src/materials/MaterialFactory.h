#pragma once
#include "../../vendor/nlohmann-json/json.hpp"
#include "Dielectric.h"
#include "DiffuseLight.h"
#include "Lambertian.h"
#include "Material.h"
#include "Metal.h"
#include <memory>
#include <string>

using nlohmann::json;

namespace rt {

  class MaterialFactory {
  public:
    static sPtr<Material> FromJson(json materialJson) {

      string matType = materialJson["type"].get<string>();

      if (matType == "lambertian")
        return std::make_shared<Lambertian>(materialJson.get<Lambertian>());

      if (matType == "metal")
        return std::make_shared<Metal>(materialJson.get<Metal>());

      if (matType == "dielectric")
        return std::make_shared<Dielectric>(materialJson.get<Dielectric>());

      if (matType == "diffuse_light")
        return std::make_shared<DiffuseLight>(materialJson. get<DiffuseLight>());

      return nullptr;
    }
  };
} // namespace rt