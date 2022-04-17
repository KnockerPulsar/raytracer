#pragma once

#include "../vendor/glm/glm/gtx/euler_angles.hpp"
#include "AABB.h"
#include "Constants.h"
#include "Defs.h"
#include "Ray.h"
#include "data_structures/vec3.h"
#include <cmath>

namespace rt {
  class Transformation {
  public:
    glm::mat4 modelMatrix;
    glm::mat4 invModelMatrix;

    glm::mat4 rotationMatrix;
    glm::mat4 invRotationMatrix;

    vec3 translate;
    vec3 rotate;

    Transformation(vec3 translation = vec3::Zero(),
                   vec3 rotation    = vec3::Zero())
        : translate(translation), rotate(rotation) {

      glm::mat4 model          = glm::mat4(1.0f);
      auto      translationMat = glm::translate(model, toGlmVec3(translation));
      // modelMatrix      = glm::scale(modelMatrix, Scale);
      rotationMatrix    = glm::eulerAngleXYZ(glm::radians(rotation.x),
                                          glm::radians(rotation.y),
                                          glm::radians(rotation.z));
      modelMatrix       = translationMat * rotationMatrix;
      invModelMatrix    = glm::inverse(modelMatrix);
      invRotationMatrix = glm::inverse(rotationMatrix);
    }

    static vec3 applyGlmMat(const vec3 &vec, glm::mat<4, 4, float> mat) {
      auto glmVec     = glm::vec4(vec.x, vec.y, vec.z, 1);
      auto rotatedVec = mat * glmVec;

      return vec3(rotatedVec.x, rotatedVec.y, rotatedVec.z);
    }

    static glm::vec3 toGlmVec3(const vec3 &vec) {
      return glm::vec3(vec.x, vec.y, vec.z);
    }

    vec3 Apply(const vec3 &inVec) const {
      return applyGlmMat(inVec, modelMatrix);
    }

    vec3 Inverse(const vec3 &inVec) const {
      return applyGlmMat(inVec, invModelMatrix);
    }

    AABB regenAABB(const AABB &aabb) const {
      // Generate all 8 vertices of the input AABB
      // Apply the transform to all 8
      // Get the bounding box of the rotated bounding box
      std::vector<vec3> vertices = {
          vec3(aabb.min.x, aabb.min.y, aabb.min.z),
          vec3(aabb.min.x, aabb.min.y, aabb.max.z),
          vec3(aabb.min.x, aabb.max.y, aabb.min.z),
          vec3(aabb.min.x, aabb.max.y, aabb.max.z),
          vec3(aabb.max.x, aabb.min.y, aabb.min.z),
          vec3(aabb.max.x, aabb.min.y, aabb.max.z),
          vec3(aabb.max.x, aabb.max.y, aabb.min.z),
          vec3(aabb.max.x, aabb.max.y, aabb.max.z),
      };

      for (auto &&vert : vertices) {
        vert = applyGlmMat(vert, modelMatrix);
      }

      AABB newAABB = {vec3(infinity), vec3(-infinity)};

      for (auto &&vert : vertices) {
        newAABB.min.x = fmin(newAABB.min.x, vert.x);
        newAABB.min.y = fmin(newAABB.min.y, vert.y);
        newAABB.min.z = fmin(newAABB.min.z, vert.z);

        newAABB.max.x = fmax(newAABB.max.x, vert.x);
        newAABB.max.y = fmax(newAABB.max.y, vert.y);
        newAABB.max.z = fmax(newAABB.max.z, vert.z);
      }

      return newAABB;
    }
  };

  // This is how you use `json.get<Box>()`
  inline void from_json(const json     &objectJson,
                        Transformation &transformation) {
    auto t = objectJson["translation"].get<vec3>();
    auto r = objectJson["rotation"].get<vec3>();
  }

  inline void to_json(json &j, const Transformation &t) {
    j = {{"transform", {{"translation", t.translate}, {"rotation", t.rotate}}}};
  }
} // namespace rt