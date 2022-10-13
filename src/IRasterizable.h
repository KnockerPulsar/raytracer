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
       vec3 r = transformation.getRotation();
       vec3 t          = transformation.getTranslation();
       rlTranslatef(t.x, t.y, t.z);
       rlRotatef(r.x, 1, 0, 0);
       rlRotatef(r.y, 0, 1, 0);
       rlRotatef(r.z, 0, 0, 1);
    }
  };
} // namespace rt