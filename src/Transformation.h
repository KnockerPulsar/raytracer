#pragma once

#include "../vendor/glm/glm/gtx/euler_angles.hpp"
#include "Defs.h"
#include "Ray.h"
#include "data_structures/vec3.h"

namespace rt {
  class Transformation {
  public:
    glm::mat4 modelMatrix;
    glm::mat4 invModelMatrix;

    Transformation(vec3 translation = vec3::Zero(),
                   vec3 rotation    = vec3::Zero()) {

      glm::mat4 model = glm::mat4(1.0f);
      auto translationMat = glm::translate(model, toGlmVec3(translation));
      // modelMatrix      = glm::scale(modelMatrix, Scale);
      auto rotMat    = glm::eulerAngleXYZ(glm::radians(rotation.x),
                                       glm::radians(rotation.y),
                                       glm::radians(rotation.z));
      modelMatrix    = translationMat * rotMat;
      invModelMatrix = glm::inverse(modelMatrix);
    }

    // virtual vec3 Apply(const vec3 &inVec) const   = 0;
    // virtual vec3 Inverse(const vec3 &inVec) const = 0;

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
    /*
      Translate example

      virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec)
    const override {

      // Apply inverse translate on ray
      Ray moved = Ray(r.origin - offset, r.direction, r.time);

      // Check for hit
      if (!ptr->Hit(moved, t_min, t_max, rec))
        return false;

      // Apply translate on hit point
      rec.p += offset;
      rec.set_face_normal(moved, rec.normal);
      return true;
    }
     */
  };
  // TODO: Figure out a way to apply transformations without wrapping the object
  // class Translation : public Transformation {
  // public:
  //   vec3 translation;

  //   Translation(vec3 translate) : translation(translate) {}

  //   virtual vec3 Apply(const vec3 &inVec) const override {
  //     return inVec + translation;
  //   }
  //   virtual vec3 Inverse(const vec3 &inVec) const override {
  //     return inVec - translation;
  //   }
  // };

  // class Rotation : public Transformation {
  // public:
  //   vec3                  eulerAngles;
  //   glm::mat<4, 4, float> rotMat;
  //   glm::mat<4, 4, float> invRotMat;

  //   Rotation(vec3 eulers) : eulerAngles(eulers) {
  //     rotMat    = glm::eulerAngleXYZ(glm::radians(eulerAngles.x),
  //                                 glm::radians(eulerAngles.y),
  //                                 glm::radians(eulerAngles.z));
  //     invRotMat = glm::transpose(rotMat);
  //   }

  //   virtual vec3 Apply(const vec3 &inVec) const override {
  //     return applyGlmMat(inVec, rotMat);
  //   }
  //   virtual vec3 Inverse(const vec3 &inVec) const override {
  //     return applyGlmMat(inVec, invRotMat);
  //   }
  // };
} // namespace rt