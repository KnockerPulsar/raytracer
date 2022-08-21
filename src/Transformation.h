#pragma once

#include "../vendor/glm/glm/gtx/euler_angles.hpp"
#include <imgui.h>
#include "editor/Utils.h"
#include "AABB.h"
#include "Constants.h"
#include "Defs.h"
#include "IImguiDrawable.h"
#include "data_structures/vec3.h"
#include <cmath>

namespace rt {
  class Transformation : public IImguiDrawable {
  public:
    vec3 translate;
    vec3 rotate;

    Transformation(vec3 translation = vec3::Zero(), vec3 rotation = vec3::Zero())
        : translate(translation), rotate(rotation) {}

    static vec3 applyGlmMat(const vec3 &vec, glm::mat<4, 4, float> mat) {
      auto glmVec     = glm::vec4(vec.x, vec.y, vec.z, 1);
      auto rotatedVec = mat * glmVec;

      return vec3(rotatedVec.x, rotatedVec.y, rotatedVec.z);
    }

    vec3 Apply(const vec3 &inVec) const { return applyGlmMat(inVec, getModelMatrix()); }

    vec3 Inverse(const vec3 &inVec) const { return applyGlmMat(inVec, getInverseModelMatrix()); }

    AABB regenAABB(const AABB &aabb) const {
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
        vert = applyGlmMat(vert, model);
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

    glm::mat4 getModelMatrix() const {
      glm::mat4 model          = glm::mat4(1.0f);
      glm::mat4 translationMat = glm::translate(model, translate.toGlm());
      glm::mat4 rotationMatrix = getRotationMatrix();

      return translationMat * rotationMatrix;
    }

    glm::mat4 getRotationMatrix() const {
      return glm::eulerAngleXYZ(glm::radians(rotate.x), glm::radians(rotate.y), glm::radians(rotate.z));
    }

    glm::mat4 getInverseModelMatrix() const { return glm::inverse(getModelMatrix()); }

    glm::mat4 getInverseRotationMatrix() const { return glm::inverse(getRotationMatrix()); }

    virtual void OnDerivedImgui() override {
      ImGui::DragFloat3(
          ("Translation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &translate.x, 0.05f);
      ImGui::DragFloat3(("Rotation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &rotate.x, 0.05f);
    }
  };

  // This is how you use `json.get<Box>()`
  inline void from_json(const json &objectJson, Transformation &transformation) {
    transformation.translate = objectJson["translation"].get<vec3>();
    transformation.rotate = objectJson["rotation"].get<vec3>();
  }

  inline void to_json(json &j, const Transformation &t) {
    j = {{"transform", {{"translation", t.translate}, {"rotation", t.rotate}}}};
  }
} // namespace rt