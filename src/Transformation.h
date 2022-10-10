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

    Transformation(vec3 translation = vec3::Zero(), vec3 rotation = vec3::Zero());

    static vec3 applyGlmMat(const vec3 &vec, glm::mat<4, 4, float> mat);

    vec3 Apply(const vec3 &inVec) const;
    vec3 Inverse(const vec3 &inVec) const;

    AABB regenAABB(const AABB &aabb) const;

    glm::mat4 getModelMatrix() const;

    glm::mat4 getRotationMatrix() const;

    glm::mat4 getInverseModelMatrix() const;

    glm::mat4 getInverseRotationMatrix() const;

    virtual void OnDerivedImgui() override;
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
