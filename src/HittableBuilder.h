#include "Defs.h"
#include "Scene.h"
#include <memory>
#include <string>

namespace rt {
  template <typename T = Hittable> class HittableBuilder {
    T *hittable;

    HittableBuilder(T *newHittable) : hittable(newHittable) {}

  public:
    template <typename... Args> HittableBuilder(Args... args) : hittable(new T(args...)) {}

    template <typename... Args> static HittableBuilder CreateHittable(Args... args) {
      return HittableBuilder(args...);
    }

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

    HittableBuilder &withMaterial(sPtr<Material> mat) {
      hittable->changeMaterial(mat);
      return *this;
    }

    sPtr<T> build() { return std::shared_ptr<T>(hittable); }
  };
} // namespace rt
