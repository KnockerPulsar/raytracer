#pragma once

#include <cmath>
#include <limits>
#include <memory>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degress) {
  return degress * pi / 180.0;
}


#include "interval.h"
#include "ray.h"
#include "vec3.h"
