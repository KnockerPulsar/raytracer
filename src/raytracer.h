#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <tuple>

#include "ray.h"

template<typename T>
using sPtr = std::shared_ptr<T>;
using std::make_shared;
using std::sqrt;
using std::tuple;

#define u8 uint8_t

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;


inline float degreesToRadians(float degrees) {
	return degrees * pi / 180.0f;
}
