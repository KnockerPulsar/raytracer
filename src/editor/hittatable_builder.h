#include "common/Hittable.h"

#include "materials/material.h"

#include <memory>
#include <string>

namespace rt::editor {
  template <typename T = rt::Hittable> class HittableBuilder {
    std::shared_ptr<T> hittable;

    HittableBuilder(T *newHittable) : hittable(newHittable) {}

  public:
    template <typename... Args> HittableBuilder(Args... args) : hittable(std::make_shared<T>(args...)) {}

    HittableBuilder &withName(std::string name) {
      hittable->name = name;
      return *this;
    }

    HittableBuilder &withTranslation(vec3 translation) {
      hittable->transformation.setTranslation(translation);
      return *this;
    }

    HittableBuilder &withRotation(vec3 rotation) {
      hittable->transformation.setRotation(rotation);
      return *this;
    }

    HittableBuilder &rotate(vec3 rotation) {
      hittable->transformation.setRotation(hittable->transformation.getRotationEuler() + rotation);
      return *this;
    }

    HittableBuilder &withMaterial(Material *mat) {
      hittable->material = std::make_shared<Material>(mat);
      return *this;
    }

    HittableBuilder &withMaterial(sPtr<rt::editor::Material> mat) {
      hittable->changeMaterial(mat);
      return *this;
    }

    operator std::shared_ptr<Hittable>() const { return hittable; }
  };
} // namespace rt
