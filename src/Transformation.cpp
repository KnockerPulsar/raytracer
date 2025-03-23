#include "Transformation.h"

#include "Constants.h"
#include "Defs.h"
#include "editor/Utils.h"

#include <imgui.h>
#include "../vendor/glm/glm/gtx/euler_angles.hpp"

#include <cmath>
#include <raylib.h>
#include <raymath.h>

void rt::Transformation::recomputeCaches() { invertedRotate = QuaternionInvert(rotate); }

rt::Transformation::Transformation(vec3 translation, vec3 rotation)
    : translate(translation), rotate(QuaternionFromEuler(rotation.x, rotation.y, rotation.z)) {
  recomputeCaches();
}

vec3 rt::Transformation::Apply(const vec3 &inVec) const {
  return translate + ApplyRotation(inVec);
}

vec3 rt::Transformation::Inverse(const vec3 &inVec) const {
  return ApplyInverseRotation(inVec - translate);
}

rt::AABB rt::Transformation::regenAABB(const AABB &aabb) const {
  // Generate all 8 vertices of the input AABB
  // Apply the transform to all 8
  // Get the bounding box of the rotated bounding box
  std::vector<vec3> vertices = {
      aabb.min,                                 // 000
      vec3(aabb.min.x, aabb.min.y, aabb.max.z), // 001
      vec3(aabb.min.x, aabb.max.y, aabb.min.z), // 010
      vec3(aabb.min.x, aabb.max.y, aabb.max.z), // 011
      vec3(aabb.max.x, aabb.min.y, aabb.min.z), // 100
      vec3(aabb.max.x, aabb.min.y, aabb.max.z), // 101
      vec3(aabb.max.x, aabb.max.y, aabb.min.z), // 110
      aabb.max,                                 // 111
  };

  for (auto &&vert : vertices) {
    vert = Apply(vert);
  }

  vec3 newMin = vec3(infinity);
  vec3 newMax = -newMin;

  for (auto &&vert : vertices) {
    newMin.x = fmin(newMin.x, vert.x);
    newMin.y = fmin(newMin.y, vert.y);
    newMin.z = fmin(newMin.z, vert.z);

    newMax.x = fmax(newMax.x, vert.x);
    newMax.y = fmax(newMax.y, vert.y);
    newMax.z = fmax(newMax.z, vert.z);
  }

  return AABB(newMin, newMax);
}

vec3 rt::Transformation::getRotationEuler() const {
  return toEulerRotation();
}

glm::vec4 rt::Transformation::getRotationAxisAngle() const {
  Vector3 outAxis;
  float outAngle;
  QuaternionToAxisAngle(rotate, &outAxis, &outAngle);
  return glm::vec4(outAxis.x, outAxis.y, outAxis.z, glm::degrees(outAngle));
}

vec3 rt::Transformation::getTranslation() const { return translate; }

namespace
{
  Quaternion fromEuler(vec3 euler) {
    auto const xQuat = QuaternionFromEuler(glm::radians(euler.x), 0, 0);
    auto const yQuat = QuaternionFromEuler(0, glm::radians(euler.y), 0);
    auto const zQuat = QuaternionFromEuler(0, 0, glm::radians(euler.z));

    return QuaternionMultiply(QuaternionMultiply(xQuat, yQuat), zQuat);
  }
}

void rt::Transformation::OnImgui() {
  auto const translationChanged =
      ImGui::DragFloat3(("Translation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &translate.x, 0.05f);

  auto euler = toEulerRotation();
  auto const originalEuler = euler;
  auto const rotationChanged =
      ImGui::DragFloat3(("Rotation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &euler.x, 0.05f);

  if (rotationChanged) {
    rotateDelta(euler - originalEuler);
  }

  if (translationChanged || rotationChanged)
    recomputeCaches();
}

void rt::Transformation::setRotation(vec3 eulerAngles) {
  fromEulerRotation(eulerAngles);
  recomputeCaches();
}

void rt::Transformation::setTranslation(vec3 tranlsation) {
  translate = tranlsation;
  recomputeCaches();
}

void rt::Transformation::rotateDelta(vec3 eulerDelta) {
  auto deltaQuaternion = fromEuler(eulerDelta);
  rotate               = QuaternionMultiply(deltaQuaternion, rotate);

  recomputeCaches();
}

void rt::from_json(const json &objectJson, Transformation &transformation) {
  transformation.translate = objectJson["translation"].get<vec3>();
  transformation.fromEulerRotation(objectJson["rotation"].get<vec3>());
  transformation.recomputeCaches();
}

void rt::to_json(json &j, const Transformation &t) {
  j = {{"transform", {{"translation", t.translate}, {"rotation", t.toEulerRotation()}}}};
}

vec3 rt::Transformation::toEulerRotation() const {
  auto const rollPitchYawRadians = QuaternionToEuler(rotate);
  return vec3(glm::degrees(rollPitchYawRadians.x), glm::degrees(rollPitchYawRadians.y),
              glm::degrees(rollPitchYawRadians.z));
}

void rt::Transformation::fromEulerRotation(vec3 euler) {
  rotate = fromEuler(euler);
}

vec3 rt::Transformation::ApplyRotation(const vec3 &inVec) const {
  return Vector3RotateByQuaternion(inVec, rotate);
}

vec3 rt::Transformation::ApplyInverseRotation(const vec3 &inVec) const {
  return Vector3RotateByQuaternion(inVec, invertedRotate);
}
