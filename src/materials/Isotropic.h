#include "../Defs.h"
#include "../materials/Material.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include <memory>
#include <raylib.h>
namespace rt {
  class Isotropic : public Material {
  public:
    sPtr<Texture> albedo;
    Isotropic(vec3 color) : albedo(std::make_shared<SolidColor>(color)) {}
    Isotropic(sPtr<Texture> tex) : albedo(tex) {}

    virtual bool scatter(const Ray &r_in, HitRecord &rec, vec3 &attenuation, Ray &scattered) const override {
      scattered   = Ray(rec.p, vec3::RandomInUnitSphere(), r_in.time);
      attenuation = albedo->Value(rec.u, rec.v, rec.p);
      return true;
    }

    json toJson() const override { return {"type", "unimplemented - isotropic"}; }

    virtual void OnImgui() override { albedo->OnImgui(); }
  };
} // namespace rt