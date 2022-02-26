#pragma once
#include <raylib.h>

namespace raytracer {
  class Clr : public Color {
  public:
    static Clr FromFloat(float ir, float ig, float ib, float ia = 1) {
      unsigned char r = (unsigned char)(ir * 255);
      unsigned char g = (unsigned char)(ig * 255);
      unsigned char b = (unsigned char)(ib * 255);
      unsigned char a = (unsigned char)(ia * 255);
      return Color{r, g, b, a};
    }

    Clr(const Color &iClr) : Color{iClr} {}
  };

} // namespace raytracer