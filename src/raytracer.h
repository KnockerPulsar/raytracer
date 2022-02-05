#pragma once

#include <cmath>
#include <limits>
#include <memory>

// Usings
using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Utility functions
inline float degress_to_radians(float degress) { return degress * pi / 180; }

# include "Ray.h"
# include "../include/raylib-cpp/include/Vector3.hpp"

typedef raylib::Vector3 vec3;
