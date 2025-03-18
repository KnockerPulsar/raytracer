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
  private:
    vec3 translate;
    vec3 rotate;

    glm::mat4 modelMatrix, inverseModelMatrix;
    glm::mat3 rotationMatrix, inverseRotationMatrix;

    friend void from_json(const json &objectJson, Transformation &transformation);
    friend void to_json(json &j, const Transformation &t);

    // Should actually never be called while raytracing.
    void recomputeCaches() {
      // Order matters. The model matrix depends on the rotation matrix.
      rotationMatrix = glm::eulerAngleXYZ(glm::radians(rotate.x), glm::radians(rotate.y), glm::radians(rotate.z));
      inverseRotationMatrix = glm::inverse(rotationMatrix);

      modelMatrix = [&] {
        auto const translationMat = glm::translate(glm::mat4(1.0f), translate.toGlm());
        auto const rotMatrix      = glm::mat4(rotationMatrix);

        assert(rotMatrix[3][3] == 1.0f);

        return translationMat * rotMatrix;
      }();
      inverseModelMatrix = glm::inverse(modelMatrix);
    }

  public:
    Transformation(vec3 translation = vec3::Zero(), vec3 rotation = vec3::Zero())
        : translate(translation), rotate(rotation) {
      recomputeCaches();
    }

    static vec3 applyGlmMat(const vec3 &vec, glm::mat<4, 4, float> mat) {
      auto glmVec     = glm::vec4(vec.x, vec.y, vec.z, 1);
      auto rotatedVec = mat * glmVec;

      return vec3(rotatedVec.x, rotatedVec.y, rotatedVec.z);
    }

    vec3 Apply(const vec3 &inVec) const { return applyGlmMat(inVec, modelMatrix); }

    vec3 Inverse(const vec3 &inVec) const { return applyGlmMat(inVec, inverseModelMatrix); }

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

      for (auto &&vert : vertices) {
        vert = applyGlmMat(vert, modelMatrix);
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

    glm::mat4 getRotationMatrix() const { return rotationMatrix; }

    glm::mat4 getInverseRotationMatrix() const { return inverseRotationMatrix; }

    vec3 getRotation() const { return rotate; }

    vec3 getTranslation() const { return translate; }

    virtual void OnImgui() override {
      auto const translationChanged =
          ImGui::DragFloat3(("Translation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &translate.x, 0.05f);
      auto const rotationChanged =
          ImGui::DragFloat3(("Rotation##" + EditorUtils::GetIDFromPointer(this)).c_str(), &rotate.x, 0.05f);

      if(translationChanged || rotationChanged)
        recomputeCaches();
    }

    void setRotation(vec3 eulerAngles) {
      rotate = eulerAngles;
      recomputeCaches();
    }

    void setTranslation(vec3 tranlsation) {
      translate = tranlsation;
      recomputeCaches();
    }
  };

  // This is how you use `json.get<Box>()`
  inline void from_json(const json &objectJson, Transformation &transformation) {
    transformation.translate = objectJson["translation"].get<vec3>();
    transformation.rotate = objectJson["rotation"].get<vec3>();
    transformation.recomputeCaches();
  }

  inline void to_json(json &j, const Transformation &t) {
    j = {{"transform", {{"translation", t.translate}, {"rotation", t.rotate}}}};
  }
} // namespace rt
