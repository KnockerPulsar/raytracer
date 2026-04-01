#pragma once

#include "AABB.h"
#include "data_structures/vec3.h"
#include <raylib.h>
#include <raymath.h>

namespace rt {
  class Transformation {
  private:
    vec3 const translate;
    // TODO can we remove one of these? Quaternion inversion should be cheap
    Quaternion const rotate;
    Quaternion const invertedRotate;

    // friend void from_json(const json &objectJson, Transformation &transformation);
    // friend void to_json(json &j, const Transformation &t);
  public:
    Transformation(vec3 translation = vec3::Zero(), vec3 rotation = vec3::Zero());

    vec3 Apply(const vec3 &inVec) const;
    vec3 Inverse(const vec3 &inVec) const;

    vec3 ApplyRotation(const vec3 &inVec) const;
    vec3 ApplyInverseRotation(const vec3 &inVec) const;
  };
  //
  // // This is how you use `json.get<Box>()`
  // void from_json(const json &objectJson, Transformation &transformation);
  //
  // void to_json(json &j, const Transformation &t);
} // namespace rt
