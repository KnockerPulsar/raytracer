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
    vec3 translate, rotate;
    glm::mat4 tMatrix, invTMatrix;
    bool changed = true;

  public:

    Transformation(vec3 translation = vec3::Zero(), vec3 rotation = vec3::Zero());

    static vec3 matMul(const glm::vec4& v, const glm::mat4& m);

    vec3 ApplyPoint(const vec3 &inVec) const;
    vec3 ApplyVec(const vec3 &inVec) const;
    vec3 InversePoint(const vec3 &inVec) const;
    vec3 InverseVec(const vec3 &inVec) const;

    AABB regenAABB(const AABB &aabb);

    glm::mat4 getModelMatrix() const;
    glm::mat4 getInverseModelMatrix() const;

    void constructMatrices();
    Transformation& setTranslation(vec3 translation);
    Transformation& setRotation(vec3 rotation);

    vec3& getTranslation() { return translate; }
    vec3& getRotation() { return rotate; }

    virtual void OnDerivedImgui() override;

    json toJson() const;
  };

  // This is how you use `json.get<Box>()`
  inline void from_json(const json &objectJson, Transformation &transformation) {
    transformation
      .setTranslation(objectJson["translation"].get<vec3>())
      .setRotation( objectJson["rotation"].get<vec3>());
  }

  inline void to_json(json &j, const Transformation &t) {
    j = t.toJson();
  }
} // namespace rt
