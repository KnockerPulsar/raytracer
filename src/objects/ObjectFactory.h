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
        return std::make_shared<Sphere>(objectJson.get<Sphere>());

      if (objType == "box")
        return std::make_shared<Box>(objectJson.get<Box>());

      // XY, XZ, YZ rects
      if (objType == "xy_rect")
        return std::make_shared<XYRect>(objectJson.get<XYRect>());
      if (objType == "xz_rect")
        return std::make_shared<XZRect>(objectJson.get<XZRect>());
      if (objType == "yz_rect")
        return std::make_shared<YZRect>(objectJson.get<YZRect>());

      return nullptr;
    }
  };
} // namespace rt