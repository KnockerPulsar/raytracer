#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

#include "../include/raylib-cpp/include/Vector3.hpp"
#include "Ray.h"

typedef raylib::Vector3 vec3;

// Usings
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Utility functions
inline float degress_to_radians(float degress) {
  return degress * pi / 180;
}

inline float random_float() {
  // Returns a random float in [0,1)
  static std::uniform_real_distribution<float> distribution(0, 1);
  static std::mt19937 generator;
  return distribution(generator);
}

inline float random_float(float min, float max) {
  return min + (max - min) * random_float();
}

inline float clamp(float x, float min, float max) {
  if (x > max)
    return max;
  if (x < min)
    return min;
  return x;
}

// Since I'm borrowing Raylib's implementation of vec3
inline vec3 random_vec3() {
  return vec3(random_float(), random_float(), random_float());
}

inline vec3 random_vec3(float min, float max) {
  return vec3(random_float(min, max), random_float(min, max),
              random_float(min, max));
}

vec3 random_in_unit_sphere() {
  while (true) {
    vec3 p = random_vec3(-1, 1);
    // No implementation for vec3.squared_lem :(
    if (p.x * p.x + p.y * p.y + p.z * p.z >= 1)
      continue;
    return p;
  }
}

inline vec3 random_unit_vec() {
  return random_in_unit_sphere().Normalize();
}

inline vec3 random_in_hemisphere(const vec3& normal) {
  vec3 in_unit_sphere = random_in_unit_sphere();
  if (Vector3DotProduct(normal, in_unit_sphere) > 0.0)
    return in_unit_sphere;
    else return -in_unit_sphere;
}