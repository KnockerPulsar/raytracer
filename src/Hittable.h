#pragma once

#include "AABB.h"
#include "Constants.h"
#include "Defs.h"
#include "IRasterizable.h"
#include "Ray.h"
#include "Transformation.h"
#include "Util.h"
#include "data_structures/vec3.h"
#include <cmath>
#include <iostream>
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/glm/gtx/string_cast.hpp"
#include <vector>
using std::shared_ptr;

namespace rt {
  class Material;

  struct HitRecord {
    vec3                 p;
    vec3                 normal;
    shared_ptr<Material> mat_ptr;
    float                t, u, v;
    bool                 front_face;

    inline void set_face_normal(const Ray &r, const vec3 &outward_normal) {
      front_face = Vector3DotProduct(outward_normal, r.direction) < 0;
      normal     = front_face ? outward_normal : outward_normal * -1;
    }
  };

  class Hittable : public IRasterizable {
  public:
    Transformation transformation;

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;

    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const = 0;

    virtual json GetJson() const {
      json derived = this->GetJsonDerived();
      json tJson   = transformation;
      derived.update(tJson);
      return derived;
    }

    virtual json GetJsonDerived() const { return {"type", "unimplemented"}; }

    virtual bool HitTransformed(const Ray &r, float t_min, float t_max, HitRecord &rec) {

      // Need to apply inverse translation on ray origin
      // And translation on ray hit point
      // Need to apply inverse rotation on both ray origin and direction
      // Need to apply rotation on both hit point and normals
      // Compare against RotateY and Translate implementations for accuracy

      Ray transformedRay = r;

      // Apply inverse transformations in reverse
      transformedRay.origin    = transformation.Inverse(r.origin);
      transformedRay.direction = Transformation::applyGlmMat(r.direction, transformation.invRotationMatrix);

      if (!this->Hit(transformedRay, t_min, t_max, rec))
        return false;

      rec.p = transformation.Apply(rec.p);
      rec.set_face_normal(transformedRay, Transformation::applyGlmMat(rec.normal, transformation.rotationMatrix));

      return true;
    }

    virtual bool BoundingBoxTransformed(float t0, float t1, AABB &outputBox) const {
      if (!this->BoundingBox(t0, t1, outputBox))
        return false;

      outputBox = transformation.regenAABB(outputBox);
      return true;
      ;
    }

    void setTransformation(vec3 translate = vec3::Zero(), vec3 rotate = vec3::Zero()) {
      // std::cout << glm::to_string(transformation.modelMatrix) << std::endl;
      transformation = Transformation(translate, rotate);
      // std::cout << glm::to_string(transformation.modelMatrix) << std::endl;
    }
  };
} // namespace rt
