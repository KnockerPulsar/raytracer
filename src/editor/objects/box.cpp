#include "box.h"

rt::editor::Box::Box(float edgeLength) : Box(vec3(edgeLength)) {}
rt::editor::Box::Box(const vec3& extents) : extents(extents) {}

// TODO proper transformation
void rt::editor::Box::Rasterize(vec3 color) { DrawCubeV(getTranslation(), extents, color.toRaylibColor()); }
