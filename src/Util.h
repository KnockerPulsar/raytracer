#pragma once
#include "Constants.h"
#include "data_structures/vec3.h"
#include <random>
#include <raylib.h>

inline float DegressToRadians(float degress) { return degress * pi / 180; }

inline float RandomFloat() {
  // Returns a random float in [0,1)
  static thread_local std::uniform_real_distribution<float> distribution(0, 1);
  static thread_local std::mt19937                          generator;
  return distribution(generator);
}

inline float RandomFloat(float min, float max) {
  return min + (max - min) * RandomFloat();
}

inline int RandomInt(int min, int max) {
  return static_cast<int>(RandomFloat(min, max + 1));
}

inline Color ColorFromFloats(float ir, float ig, float ib, float ia = 1) {
  unsigned char r = (unsigned char)(ir * 255);
  unsigned char g = (unsigned char)(ig * 255);
  unsigned char b = (unsigned char)(ib * 255);
  unsigned char a = (unsigned char)(ia * 255);
  return Color{r, g, b, a};
}
