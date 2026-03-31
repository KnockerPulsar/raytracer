#include "plane.h"

rt::editor::Plane::Plane(float w, float h) : w(w), h(h) {}

void rt::editor::Plane::Rasterize(vec3 color) {
  rlDisableBackfaceCulling();

  DrawPlane(getTranslation(), {w, h}, color.toRaylibColor());

  rlEnableBackfaceCulling();
}
