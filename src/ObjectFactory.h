#pragma once

#include "Box.h"
#include "Defs.h"
#include "Ray.h"
#include "Sphere.h"
#include <memory>

namespace raytracer {
  class ObjectFactory {
  public:
    static sPtr<Hittable> FromJson(nlohmann::json objectJson) {
      string objType = objectJson["type"].get<string>();
      
      if (objType == "sphere")
        return std::make_shared<Sphere>(objectJson);
      if(objType == "box")
        return std::make_shared<Box>(objectJson);

      return nullptr;
    }
  };
} // namespace raytracer