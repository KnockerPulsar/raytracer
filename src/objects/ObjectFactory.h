#pragma once

#include "../Defs.h"
#include "../Ray.h"
#include "../objects/AARect.h"
#include "../objects/Box.h"
#include "../objects/Sphere.h"
#include <memory>

namespace rt {

  void to_json(json &j, const XYRect &xy) { j = xy.GetJson(); }
  void from_json(const json &j, XYRect &plane) {
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x0             = min.x;
    plane.x1             = max.x;
    plane.y0             = min.y;
    plane.y1             = max.y;
    plane.z              = center.z;
    plane.transformation = j["transform"].get<Transformation>();
  }

  void to_json(json &j, const XZRect &xz) { j = xz.GetJson(); }
  void from_json(const json &j, XZRect &plane) {
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x0             = min.x;
    plane.x1             = max.x;
    plane.z0             = min.z;
    plane.z1             = max.z;
    plane.y              = center.y;
    plane.transformation = j["transform"].get<Transformation>();
  }
  void to_json(json &j, const YZRect &yz) { j = yz.GetJson(); }
  void from_json(const json &j, YZRect &plane) {
    vec3 center  = j["pos"].get<vec3>();
    vec3 extents = j["extents"].get<vec3>();
    plane.mp     = MaterialFactory::FromJson(j["material"]);

    vec3 min = center - extents / 2;
    vec3 max = center + extents / 2;

    plane.x              = center.x;
    plane.z0             = min.z;
    plane.z1             = max.z;
    plane.y0             = min.y;
    plane.y1             = max.y;
    plane.transformation = j["transform"].get<Transformation>();
  }

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