#pragma once
#include "Lambertian.h"
#include "Dielectric.h"
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

      return nullptr;
    }
  };
} // namespace raytracer