#pragma once

#include "editor/IImguiDrawable.h"

#include "common/Transformation.h"
#include "data_structures/vec3.h"

namespace rt::editor {
  class Transformation: public IImguiDrawable {
  public:
    virtual void OnImgui() override;

    vec3 Apply(const vec3 &inVec) const;
    vec3 Inverse(const vec3 &inVec) const;

    vec3 ApplyRotation(const vec3 &inVec) const;
    vec3 ApplyInverseRotation(const vec3 &inVec) const;

    void setTranslation(vec3 translation);
    vec3 getTranslation() const;

    void setRotationEuler(vec3 eulerRotation);
    vec3 getRotationEuler() const;
    glm::vec4 getRotationAxisAngle() const;
    void rotateDelta(vec3 euler);

    auto toRaytracerTransformation() const -> rt::Transformation;
  private:
    vec3 translate{0, 0, 0};
    Quaternion rotate{QuaternionIdentity()};
  };
};
