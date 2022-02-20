#include <raylib.h>

class Clr : public Color {
 public:
  static Clr FromFloat(float ir, float ig, float ib, float ia = 1) {
    unsigned char r = (unsigned char)(ir * 255), g = (unsigned char)(ig * 255),
                  b = (unsigned char)(ib * 255), a = (unsigned char)(ia * 255);
    return Color{r, g, b, a};
  }

  Clr(const Color& iClr) : Color{iClr} {}
};
