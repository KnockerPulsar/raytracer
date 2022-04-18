#pragma once
#include "../Defs.h"
#include "../Hittable.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include "Material.h"

namespace rt {

  class DiffuseLight : public Material {
  public:
    sPtr<Texture> emit;

    DiffuseLight() = default;

    DiffuseLight(sPtr<Texture> a) : emit(a) {}

    DiffuseLight(vec3 c) : emit(std::make_shared<SolidColor>(c)) {}
    
    virtual bool scatter(const Ray &r_in, HitRecord &rec, vec3 &attenuation,
                         Ray &scattered) const override {
      return false;
    }

    virtual vec3 emitted(float u, float v, const vec3 &p) const override {
      return emit->Value(u, v, p);
    }

    json GetJson() const override {
      return json{{"type", "diffuse_light"}, {"texture", emit->GetJson()}};
    }
  };

  inline void from_json(const json &j, DiffuseLight &dl) {
    dl.emit = TextureFactory::FromJson(j["texture"]);
  }

  inline void to_json(json &j, const DiffuseLight &dl) { j = dl.GetJson(); }
} // namespace rt