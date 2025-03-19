#pragma once
#include "Transformation.h"
#include <rlgl.h>
#include "imgui.h"

/**
 * @brief Interface that rasterizable objects should inherit from.
 */
namespace rt {
  class IRasterizable {
  public:
    std::string name;

    virtual void Rasterize(vec3 color){};
    virtual void RasterizeTransformed(Transformation t, vec3 color){
      rlPushMatrix();
      Transform(t);
      Rasterize(color);
      rlPopMatrix();
    };

    static void  Transform(Transformation transformation) {
       auto rotationAxisAngle = transformation.getRotationAxisAngle();
       auto [x, y, z]          = transformation.getTranslation();
       rlTranslatef(x, y, z);
       rlRotatef(rotationAxisAngle.w, rotationAxisAngle.x, rotationAxisAngle.y, rotationAxisAngle.z);
    }
  };
} // namespace rt
