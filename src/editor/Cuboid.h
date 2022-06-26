#pragma once
#include "raylib.h"

namespace Editor {
  struct Cuboid {
    Vector3 position;
    Color   color;
    float   height;
  };
} // namespace Editor