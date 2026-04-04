#include "Transformation.h"

#include <glm/gtx/euler_angles.hpp>
#include <imgui.h>
#include <raylib.h>
#include <raymath.h>

rt::Transformation::Transformation(vec3 translation, vec3 rotationEuler)
    : translate(translation), rotate(QuaternionFromEuler(glm::radians(rotationEuler.x), glm::radians(rotationEuler.y),
                                                         glm::radians(rotationEuler.z))),
      invertedRotate(QuaternionInvert(rotate)) {}

vec3 rt::Transformation::Apply(const vec3 &inVec) const {
  return translate + ApplyRotation(inVec);
}

vec3 rt::Transformation::Inverse(const vec3 &inVec) const {
  return ApplyInverseRotation(inVec - translate);
}

vec3 rt::Transformation::ApplyRotation(const vec3 &inVec) const {
  return Vector3RotateByQuaternion(inVec, rotate);
}

vec3 rt::Transformation::ApplyInverseRotation(const vec3 &inVec) const {
  return Vector3RotateByQuaternion(inVec, invertedRotate);
}

// void rt::from_json(const json &objectJson, Transformation &transformation) {
//   transformation.translate = objectJson["translation"].get<vec3>();
//   transformation.fromEulerRotation(objectJson["rotation"].get<vec3>());
//   transformation.recomputeCaches();
// }
//
// void rt::to_json(json &j, const Transformation &t) {
//   j = {{"transform", {{"translation", t.translate}, {"rotation", t.toEulerRotation()}}}};
// }
