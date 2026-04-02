#include "transformation.h"

#include "editor/Utils.h"

#include <imgui.h>
#include <raymath.h>

namespace
{
  auto quaternionFromEuler(vec3 euler) -> Quaternion
  {
    auto const xQuat = QuaternionFromEuler(glm::radians(euler.x), 0, 0);
    auto const yQuat = QuaternionFromEuler(0, glm::radians(euler.y), 0);
    auto const zQuat = QuaternionFromEuler(0, 0, glm::radians(euler.z));

    return QuaternionMultiply(QuaternionMultiply(xQuat, yQuat), zQuat);
  }
}

// We only support translation and rotation
vec3 rt::editor::Transformation::Apply(const vec3 &inVec) const {
  return translate + ApplyRotation(inVec);
}

vec3 rt::editor::Transformation::Inverse(const vec3 &inVec) const {
  return ApplyInverseRotation(inVec - translate);
}

vec3 rt::editor::Transformation::ApplyRotation(const vec3 &inVec) const {
  return Vector3RotateByQuaternion(inVec, rotate);
}

vec3 rt::editor::Transformation::ApplyInverseRotation(const vec3 &inVec) const {
  return Vector3RotateByQuaternion(inVec, QuaternionInvert(rotate));
}

void rt::editor::Transformation::setTranslation(vec3 translation) { translate = translation; }
vec3 rt::editor::Transformation::getTranslation() const { return translate; }

void rt::editor::Transformation::setRotationEuler(vec3 eulerRotation) { rotate = quaternionFromEuler(eulerRotation); }

vec3 rt::editor::Transformation::getRotationEuler() const {
  auto const rollPitchYawRadians = QuaternionToEuler(rotate);
  return vec3(glm::degrees(rollPitchYawRadians.x), glm::degrees(rollPitchYawRadians.y),
              glm::degrees(rollPitchYawRadians.z));
}

void rt::editor::Transformation::rotateDelta(vec3 euler) {
  auto const delta = quaternionFromEuler(euler);
  rotate = QuaternionMultiply(delta, rotate);
}

glm::vec4 rt::editor::Transformation::getRotationAxisAngle() const {
  Vector3 outAxis;
  float outAngle;
  QuaternionToAxisAngle(rotate, &outAxis, &outAngle);
  return glm::vec4(outAxis.x, outAxis.y, outAxis.z, glm::degrees(outAngle));
}

auto rt::editor::Transformation::toRaytracerTransformation() const -> rt::Transformation {
  return rt::Transformation(translate, getRotationEuler());
};

void rt::editor::Transformation::OnImgui() {
  auto const translationChanged =
      ImGui::DragFloat3(("Translation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &translate.x, 0.05f);

  auto euler = getRotationEuler();
  auto const originalEuler = euler;
  auto const rotationChanged =
      ImGui::DragFloat3(("Rotation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &euler.x, 0.05f);

  if (rotationChanged) {
    rotateDelta(euler - originalEuler);
  }
}
