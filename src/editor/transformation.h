#pragma once

#include "editor/IImguiDrawable.h"
#include "data_structures/vec3.h"

namespace rt::editor {
  class Transformation: public IImguiDrawable {
  public:
    void setTranslation(vec3 translation);
    vec3 getTranslation() const;

    vec3 getRotationEuler() const;
    void rotateDelta(vec3 euler);

    virtual void OnImgui() override;

  private:
    vec3 toEulerRotation() const;

    vec3 translate;
    Quaternion rotate;
  };
};
