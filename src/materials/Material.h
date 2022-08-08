#pragma once

#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include <memory>

namespace rt {

  enum MaterialTypes { Diffuse, Dielectrical , Metallic, Emissive, MaterialTypesCount };

  struct HitRecord;

  class Material : public IImguiDrawable {
  public:
    sPtr<Texture> albedo;
  
    virtual vec3 emitted(float u, float v, const vec3 &p) const { return vec3::Zero(); }

    virtual bool scatter(const Ray &r_in, HitRecord &rec, vec3 &attenuation, Ray &scattered) const = 0;

    virtual json toJson() const = 0;
  
    Material& setTexture(sPtr<Texture> texture) {
      albedo = texture;
      return *this;
    }

    Material& setTexture(vec3 color) {
      albedo = ms<SolidColor>(color);
      return *this;
    }
  };
}; // namespace rt
