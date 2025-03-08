#pragma once

#include "AABB.h"
#include "Constants.h"
#include "Defs.h"
#include "IRasterizable.h"
#include "Ray.h"
#include "Transformation.h"
#include "Util.h"
#include "app.h"
#include "data_structures/vec3.h"
#include "editor/Utils.h"
#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>

#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/glm/gtx/string_cast.hpp"
#include "imgui.h"
#include "materials/Material.h"
#include <vector>

#include "editor/editor.h"

using std::shared_ptr, std::vector, std::optional, std::nullopt, std::make_optional, std::make_shared;

namespace rt {
  class Material;

  struct HitRecord {
    vec3                 p;
    vec3                 normal;
    shared_ptr<Material> mat_ptr;
    float                t, u, v;
    bool                 front_face;
    Hittable            *closestHit = nullptr;

    inline void set_face_normal(const Ray &r, const vec3 &outward_normal) {
      front_face = Vector3DotProduct(outward_normal, r.direction) < 0;
      normal     = front_face ? outward_normal : outward_normal * -1;
    }
  };

  class Hittable : public IRasterizable, public IImguiDrawable {
  public:
    std::string    name;
    Transformation transformation;
    sPtr<Material> material = nullptr; // Should only exist for raytracable objects

    virtual bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
    virtual bool BoundingBox(float t0, float t1, AABB &outputBox) const            = 0;

    // Specifies specific behaviour that each hittable can implement for json conversion
    virtual json toJsonSpecific() const { return {{"type", "unimplemented"}}; }

    // Specifies common behaviour all hittables share when converting to json
    virtual json toJson() const {
      json derived = this->toJsonSpecific();
      derived.update({{"name", name}});
      derived.update(transformation);
      derived.update({{"material", material->toJson()}});
      return derived;
    }

    // Transforms rays opposite the object's transform
    // Such that the object is in the correct position relative to the ray
    // Handles modifying the hit result for correct results
    // TODO: Add support for scaling? Normals will require some work to get right.
    virtual bool HitTransformed(const Ray &r, float t_min, float t_max, HitRecord &rec) const {

      // Need to apply inverse translation on ray origin
      // And translation on ray hit point
      // Need to apply inverse rotation on both ray origin and direction
      // Need to apply rotation on both hit point and normals
      // Compare against RotateY and Translate implementations for accuracy

      Ray transformedRay = r;

      // Apply inverse transformations in reverse
      transformedRay.origin    = transformation.Inverse(r.origin);
      transformedRay.direction = Transformation::applyGlmMat(r.direction, transformation.getInverseRotationMatrix());

      if (!this->Hit(transformedRay, t_min, t_max, rec))
        return false;

      rec.p = transformation.Apply(rec.p);
      rec.set_face_normal(transformedRay, Transformation::applyGlmMat(rec.normal, transformation.getRotationMatrix()));

      return true;
    }

    virtual void OnImgui() override {
      transformation.OnImgui();
      ImGui::Spacing();
      if (material) {

        auto newMat = Editor::MaterialChanger();
        if (newMat.has_value()) {
          changeMaterial(newMat.value());
        }

        ImGui::Spacing();

        material->OnImgui();
      }
    }

    void setTransformation(vec3 translate = vec3::Zero(), vec3 rotate = vec3::Zero()) {
      transformation = Transformation(translate, rotate);
    }

    virtual vector<sPtr<Hittable>> getChildrenAsList() { return vector<sPtr<Hittable>>{}; }

    virtual vector<AABB> getChildrenAABBs() {
      AABB outputBox;
      if (this->BoundingBox(0, 1, outputBox))
        return vector<AABB>{outputBox};

      return vector<AABB>();
    }

    // Only HittableLists and BVHNodes should override this.
    virtual Hittable *addChild(sPtr<Hittable> newChild) { return nullptr; }
    virtual Hittable *removeChild(sPtr<Hittable> childToRemove) { return nullptr; }

    // Should be overridden by group hittables such as
    // Boxes, HittableLists if the contain primtivies of one object, etc ...
    virtual void changeMaterial(sPtr<Material> &newMat) { material = newMat; }
  };
} // namespace rt
