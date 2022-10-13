#pragma once
#include "Transformation.h"
#include <rlgl.h>
#include "../vendor/rlImGui/imgui/imgui.h"

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
       auto [rotX, rotY, rotZ] = transformation.getRotation();
       auto [x, y, z]          = transformation.getTranslation();
       rlTranslatef(x, y, z);
       rlRotatef(rotX, 1, 0, 0);
       rlRotatef(rotY, 0, 1, 0);
       rlRotatef(rotZ, 0, 0, 1);
    }
  };
} // namespace rt