#include "objects/object.h"

#include "materials/material.h"

#include <memory>
#include <string>

// TODO rename HittableBuilder
namespace rt::editor {
  template <typename T = rt::editor::Object> class HittableBuilder {
    std::shared_ptr<T> hittable;

    HittableBuilder(T *newHittable) : hittable(newHittable) {}

  public:
    template <typename... Args> HittableBuilder(Args... args) : hittable(std::make_shared<T>(args...)) {}

    HittableBuilder &withName(std::string name) {
      hittable->setName(name);
      return *this;
    }

    HittableBuilder &withTranslation(vec3 translation) {
      hittable->getTransformation().setTranslation(translation);
      return *this;
    }

    HittableBuilder &withRotation(vec3 rotation) {
      hittable->getTransformation().setRotationEuler(rotation);
      return *this;
    }

    HittableBuilder &rotate(vec3 rotation) {
      hittable->getTransformation().rotateDelta(rotation);
      return *this;
    }

    HittableBuilder &withMaterial(sPtr<rt::editor::Material> mat) {
      hittable->setMaterial(mat);
      return *this;
    }

    operator std::shared_ptr<Object>() const { return hittable; }
  };
} // namespace rt
