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

// rt::AABB rt::Transformation::regenAABB(const AABB &aabb) const {
//   // Generate all 8 vertices of the input AABB
//   // Apply the transform to all 8
//   // Get the bounding box of the rotated bounding box
//   std::vector<vec3> vertices = {
//       aabb.min,                                 // 000
//       vec3(aabb.min.x, aabb.min.y, aabb.max.z), // 001
//       vec3(aabb.min.x, aabb.max.y, aabb.min.z), // 010
//       vec3(aabb.min.x, aabb.max.y, aabb.max.z), // 011
//       vec3(aabb.max.x, aabb.min.y, aabb.min.z), // 100
//       vec3(aabb.max.x, aabb.min.y, aabb.max.z), // 101
//       vec3(aabb.max.x, aabb.max.y, aabb.min.z), // 110
//       aabb.max,                                 // 111
//   };
//
//   for (auto &&vert : vertices) {
//     vert = Apply(vert);
//   }
//
//   vec3 newMin = vec3(rt::constants::infinity);
//   vec3 newMax = -newMin;
//
//   for (auto &&vert : vertices) {
//     newMin.x = fmin(newMin.x, vert.x);
//     newMin.y = fmin(newMin.y, vert.y);
//     newMin.z = fmin(newMin.z, vert.z);
//
//     newMax.x = fmax(newMax.x, vert.x);
//     newMax.y = fmax(newMax.y, vert.y);
//     newMax.z = fmax(newMax.z, vert.z);
//   }
//
//   return AABB(newMin, newMax);
// }

// void rt::from_json(const json &objectJson, Transformation &transformation) {
//   transformation.translate = objectJson["translation"].get<vec3>();
//   transformation.fromEulerRotation(objectJson["rotation"].get<vec3>());
//   transformation.recomputeCaches();
// }
//
// void rt::to_json(json &j, const Transformation &t) {
//   j = {{"transform", {{"translation", t.translate}, {"rotation", t.toEulerRotation()}}}};
// }
