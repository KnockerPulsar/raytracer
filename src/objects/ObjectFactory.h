#pragma once

#include "../Defs.h"
#include "../Ray.h"
#include "../objects/AARect.h"
#include "../objects/Box.h"
#include "../objects/Sphere.h"
#include <memory>

namespace rt {
  class ObjectFactory {
  public:
    static sPtr<Hittable> FromJson(nlohmann::json objectJson) {
      string objType = objectJson["type"].get<string>();

      if (objType == "sphere")
        return std::make_shared<Sphere>(objectJson);
      if (objType == "box") {
        Box b = objectJson.get<Box>();
        return std::make_shared<Box>(b);
      }
      // XY, XZ, YZ rects
      if (objType == "xz_rect")
        return std::make_shared<XZRect>(objectJson);

      return nullptr;
    }
  };
} // namespace rt