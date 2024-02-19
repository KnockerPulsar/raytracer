#pragma once

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>
#include <random>

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double pi       = 3.1415926535897932385;

inline double degrees_to_radians(double degress) { return degress * pi / 180.0; }

inline double random_double() {
#if 1
    // Returns a random real in [0, 1)
    return rand() / (RAND_MAX + 1.0);
#else
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
#endif
}

inline double random_double(double min, double max) {
    // Returns a random real in [min, max)
    return min + (max - min) * random_double();
}

#include "interval.h"
#include "ray.h"
#include "vec3.h"
