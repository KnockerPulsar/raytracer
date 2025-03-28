#pragma once

#include "AABB.h"
#include "IImguiDrawable.h"
#include "data_structures/vec3.h"
#include <raylib.h>
#include <raymath.h>

namespace rt {
  class Transformation : public IImguiDrawable {
  private:
    vec3 translate;
    Quaternion rotate;
    Quaternion invertedRotate;

    friend void from_json(const json &objectJson, Transformation &transformation);
    friend void to_json(json &j, const Transformation &t);

    // Should actually never be called while raytracing.
    void recomputeCaches();

    vec3 toEulerRotation() const;
    void fromEulerRotation(vec3);
  public:
    Transformation(vec3 translation = vec3::Zero(), vec3 rotation = vec3::Zero());

    static vec3 applyGlmMat(const vec3 &vec, glm::mat<4, 4, float> mat);

    vec3 Apply(const vec3 &inVec) const;
    vec3 Inverse(const vec3 &inVec) const;

    vec3 ApplyRotation(const vec3 &inVec) const;
    vec3 ApplyInverseRotation(const vec3 &inVec) const;

    AABB regenAABB(const AABB &aabb) const;

    // Angles in degrees
    vec3 getRotationEuler() const;
    glm::vec4 getRotationAxisAngle() const;
    vec3 getTranslation() const;

    virtual void OnImgui() override;
    void setRotation(vec3 eulerAngles);
    void setTranslation(vec3 tranlsation);
    void rotateDelta(vec3 eulerDelta);
  };

  // This is how you use `json.get<Box>()`
  void from_json(const json &objectJson, Transformation &transformation);

  void to_json(json &j, const Transformation &t);
} // namespace rt
