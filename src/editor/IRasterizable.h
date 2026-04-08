#pragma once

#include "editor/transformation.h"

#include <rlgl.h>

/**
 * @brief Interface that rasterizable objects should inherit from.
 */
namespace rt::editor {
  class IRasterizable {
  public:
    virtual void RasterizeWithoutTransformation(vec3 color) = 0;
    virtual void RasterizeTransformed(Transformation t, vec3 color){
      rlPushMatrix();
      Transform(t);
      RasterizeWithoutTransformation(color);
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
