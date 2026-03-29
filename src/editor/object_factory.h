#pragma once

#include "editor/hittable.h"

namespace rt::editor {
  class ObjectFactory {
  public:
    static std::shared_ptr<Hittable> FromJson(nlohmann::json objectJson) {
    //   std::string objType = objectJson["type"].get<std::string>();
    //
    //   if (objType == "sphere")
    //     return std::make_shared<Sphere>(objectJson.get<Sphere>());
    //
    //   if (objType == "box")
    //     return std::make_shared<Box>(objectJson.get<Box>());
    //
    //   // XY, XZ, YZ rects
    //   if (objType == "xy_rect")
    //     return std::make_shared<XYRect>(objectJson.get<XYRect>());
    //   if (objType == "xz_rect")
    //     return std::make_shared<XZRect>(objectJson.get<XZRect>());
    //   if (objType == "yz_rect")
    //     return std::make_shared<YZRect>(objectJson.get<YZRect>());
    //
    //   if (objType == "plane")
    //     return std::make_shared<Plane>(objectJson.get<Plane>());
    //
      return nullptr;
    }
  };
} // namespace rt
