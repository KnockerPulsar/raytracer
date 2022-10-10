#include "Transformation.h"

namespace rt {

  Transformation::Transformation(vec3 translation , vec3 rotation )
        : translate(translation), rotate(rotation) {}

    vec3 Transformation::matMul(const glm::vec4& v, const glm::mat4& m) {
      return vec3( m * v );
    }

    vec3 Transformation::ApplyPoint(const vec3 &inVec) const { return matMul(inVec.toPoint(), getModelMatrix()); }
    vec3 Transformation::InversePoint(const vec3 &inVec) const { return matMul(inVec.toPoint(), getInverseModelMatrix()); }

    vec3 Transformation::ApplyVec(const vec3 &inVec) const { return matMul(inVec.toVec(), getModelMatrix()); }
    vec3 Transformation::InverseVec(const vec3 &inVec) const { return matMul(inVec.toVec(), getInverseModelMatrix()); }

    AABB Transformation::regenAABB(const AABB &aabb) const {
      // Generate all 8 vertices of the input AABB
      // Apply the transform to all 8
      // Get the bounding box of the rotated bounding box
      std::vector<vec3> vertices = {
          aabb.min,                                   // 000
          vec3(aabb.min.x, aabb.min.y, aabb.max.z),   // 001
          vec3(aabb.min.x, aabb.max.y, aabb.min.z),   // 010
          vec3(aabb.min.x, aabb.max.y, aabb.max.z),   // 011
          vec3(aabb.max.x, aabb.min.y, aabb.min.z),   // 100
          vec3(aabb.max.x, aabb.min.y, aabb.max.z),   // 101
          vec3(aabb.max.x, aabb.max.y, aabb.min.z),   // 110
          aabb.max,                                   // 111
      };

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

    glm::mat4 Transformation::getModelMatrix() const {
      glm::mat4 model          = glm::mat4(1.0f);
      glm::mat4 translationMat = glm::translate(model, translate.toGlm());
      glm::mat4 rotationMatrix = getRotationMatrix();

      return translationMat * rotationMatrix;
    }

    glm::mat4 Transformation::getRotationMatrix() const {
      return glm::eulerAngleXYZ(glm::radians(rotate.x), glm::radians(rotate.y), glm::radians(rotate.z));
    }

    glm::mat4 Transformation::getInverseModelMatrix() const { return glm::inverse(getModelMatrix()); }

    glm::mat4 Transformation::getInverseRotationMatrix() const { return glm::inverse(getRotationMatrix()); }

    void Transformation::OnDerivedImgui() {
      ImGui::DragFloat3(
          ("Translation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &translate.x, 0.05f);
      ImGui::DragFloat3(("Rotation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &rotate.x, 0.05f);
    }
}
