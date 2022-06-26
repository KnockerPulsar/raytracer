#pragma once

#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include <memory>

namespace rt {

  enum MaterialTypes { Diffuse, Dielectrical , Metallic, Emissive, MaterialTypesCount };

  struct HitRecord;

  class Material : public IImguiDrawable {
  public:
    virtual vec3 emitted(float u, float v, const vec3 &p) const { return vec3::Zero(); }

    virtual bool scatter(const Ray &r_in, HitRecord &rec, vec3 &attenuation, Ray &scattered) const = 0;

    virtual json GetJson() const = 0;
  };
}; // namespace rt
