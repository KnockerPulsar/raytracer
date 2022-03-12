#pragma once
#include <random>
#include "Constants.h"
#include "Vec3.h"

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

inline int RandomInt(int min, int max){
  return static_cast<int>(RandomFloat(min,max+1));
}
