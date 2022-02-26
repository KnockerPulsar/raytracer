#pragma once
#include <random>
#include "Constants.h"

inline float DegressToRadians(float degress) {
  return degress * pi / 180;
}

inline float RandomFloat() {
  // Returns a random float in [0,1)
  static thread_local std::uniform_real_distribution<float> distribution(0, 1);
  static thread_local std::mt19937 generator;
  return distribution(generator);
}

inline float RandomFloat(float min, float max) {
  return min + (max - min) * RandomFloat();
}

