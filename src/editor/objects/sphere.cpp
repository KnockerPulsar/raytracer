#include "sphere.h"

rt::editor::Sphere::Sphere(float radius) : radius(radius) {}
void rt::editor::Sphere::Rasterize(vec3 color) {
  DrawSphere(getTranslation(), radius, color.toRaylibColor());
}
