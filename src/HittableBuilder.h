#include "Defs.h"
#include "Scene.h"
#include <memory>
#include <string>

namespace rt {
  template <typename T = Hittable> class HittableBuilder {
    std::shared_ptr<T> hittable;

    HittableBuilder(T *newHittable) : hittable(newHittable) {}

  public:
    template <typename... Args> HittableBuilder(Args... args) : hittable(std::make_shared<T>(args...)) {}

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

    operator std::shared_ptr<Hittable>() const { return hittable; }
  };
} // namespace rt
