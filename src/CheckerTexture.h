#include "SolidColor.h"
#include "Vec3.h"
#include <future>
#include <memory>
#include <raylib.h>
namespace raytracer {
  class CheckerTexture : public Texture {
  private:
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;

  public:
    CheckerTexture() = default;
    CheckerTexture(std::shared_ptr<Texture> _even,
                   std::shared_ptr<Texture> _odd)
        : even(_even), odd(_odd) {}

    CheckerTexture(Vec3 c1, Vec3 c2)
        : even(std::make_shared<SolidColor>(c1)),
          odd(std::make_shared<SolidColor>(c2)) {}

    virtual Vec3 Value(float u, float v, const Vec3 &p) const override {
      float sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
      if (sines < 0)
        return odd->Value(u, v, p);
      else
        return even->Value(u, v, p);
    }
  };
} // namespace raytracer