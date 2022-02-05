#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

# include "Ray.h"
# include "../include/raylib-cpp/include/Vector3.hpp"

typedef raylib::Vector3 vec3;

// Usings
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Utility functions
inline float degress_to_radians(float degress) { return degress * pi / 180; }

inline float random_float() {
    // Returns a random float in [0,1)
    static std::uniform_real_distribution<float> distribution(0,1);
    static std::mt19937 generator;
    return distribution(generator);
}

inline float random_float(float min, float max){
    return min + (max-min)*random_float();
}

inline float clamp(float x, float min, float max){
    if(x > max) return max;
    if(x < min) return min;
    return x;
}