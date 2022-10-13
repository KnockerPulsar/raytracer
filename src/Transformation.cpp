#include "Transformation.h"
#include "objects/ConstantMedium.h"
#include <cassert>
#include <ostream>
#include <utility>

namespace rt {

  Transformation::Transformation(vec3 translation, vec3 rotation) : translate(translation), rotate(rotation) {
    constructMatrices();
  }

    vec3 Transformation::matMul(const glm::vec4& v, const glm::mat4& m) {
      return vec3( m * v );
    }

    vec3 Transformation::ApplyPoint(const vec3 &inVec) const { return matMul(inVec.toPoint(), tMatrix); }
    vec3 Transformation::InversePoint(const vec3 &inVec) const { return matMul(inVec.toPoint(), invTMatrix); }

    vec3 Transformation::ApplyVec(const vec3 &inVec) const { return matMul(inVec.toVec(), tMatrix); }
    vec3 Transformation::InverseVec(const vec3 &inVec) const { return matMul(inVec.toVec(), invTMatrix); }

    AABB Transformation::regenAABB(const AABB &aabb) {
      // Generate all 8 vertices of the input AABB
      // Apply the transform to all 8
      // Get the bounding box of the rotated bounding box
      std::vector<vec3> vertices = {
          aabb.b3.min,                                   // 000
          vec3(aabb.b3.min.x, aabb.b3.min.y, aabb.b3.max.z),   // 001
          vec3(aabb.b3.min.x, aabb.b3.max.y, aabb.b3.min.z),   // 010
          vec3(aabb.b3.min.x, aabb.b3.max.y, aabb.b3.max.z),   // 011
          vec3(aabb.b3.max.x, aabb.b3.min.y, aabb.b3.min.z),   // 100
          vec3(aabb.b3.max.x, aabb.b3.min.y, aabb.b3.max.z),   // 101
          vec3(aabb.b3.max.x, aabb.b3.max.y, aabb.b3.min.z),   // 110
          aabb.b3.max,                                   // 111
      };

      constructMatrices();
      glm::mat4 model = getModelMatrix();
      for (auto &&vert : vertices) {
        vert = matMul(vert.toPoint(), model);
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

    glm::mat4 Transformation::getModelMatrix() const { return tMatrix; }

    glm::mat4 Transformation::getInverseModelMatrix() const {
      return invTMatrix;
    }

    void Transformation::constructMatrices() {
        glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), translate.v);

        glm::vec3 r              = glm::radians(rotate.v);
        glm::mat4 rotationMatrix = glm::eulerAngleXYZ(r.x, r.y, r.z);

        tMatrix = translationMat * rotationMatrix;
        invTMatrix = glm::inverse(tMatrix);
    }

    Transformation& Transformation::setTranslation(vec3 translation) {
      translate = translation;
      changed = true;

      return *this;
    }

    Transformation& Transformation::setRotation(vec3 rotation) {
      rotate = rotation;
      changed = true;

      return *this;
    }

    void Transformation::OnDerivedImgui() {
      ImGui::DragFloat3(
          ("Translation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &translate.x, 0.05f);
      ImGui::DragFloat3(("Rotation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &rotate.x, 0.05f);
    }

    json Transformation::toJson() const {
      return {{"transform", {{"translation", translate}, {"rotation", rotate}}}};
    }
}
