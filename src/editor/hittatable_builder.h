#include "hittable.h"

#include "materials/material.h"

#include <memory>
#include <string>

namespace rt::editor {

  template <typename T = rt::editor::Hittable> class HittableBuilder {
    std::shared_ptr<T> hittable;

    HittableBuilder(T *newHittable) : hittable(newHittable) {}

  public:
    template <typename... Args> HittableBuilder(Args... args) : hittable(std::make_shared<T>(args...)) {}

    HittableBuilder &withName(std::string name) {
      hittable->setName(name);
      return *this;
    }

    HittableBuilder &withTranslation(vec3 translation) {
      hittable->setTranslation(translation);
      return *this;
    }

    HittableBuilder &withRotation(vec3 rotation) {
      hittable->setRotationEuler(rotation);
      return *this;
    }

    HittableBuilder &rotate(vec3 rotation) {
      hittable->rotateDelta(rotation);
      return *this;
    }

    HittableBuilder &withMaterial(sPtr<rt::editor::Material> mat) {
      hittable->setMaterial(mat);
      return *this;
    }

    operator std::shared_ptr<Hittable>() const { return hittable; }
  };
} // namespace rt
