#pragma once
#include "../Defs.h"
#include "../Hittable.h"
#include "../textures/SolidColor.h"
#include "../textures/Texture.h"
#include "../textures/TextureFactory.h"
#include "Material.h"


namespace raytracer {

  class DiffuseLight : public Material {
  public:
    sPtr<Texture> emit;

    DiffuseLight(sPtr<Texture> a) : emit(a) {}

    DiffuseLight(Vec3 c) : emit(std::make_shared<SolidColor>(c)) {}

    DiffuseLight(json materialJson){
      emit = TextureFactory::FromJson(materialJson["texture"]);
    }

    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const override {
      return false;
    }

    virtual Vec3 emitted(float u, float v, const Vec3 &p) const override {
      return emit->Value(u, v, p);
    }
  };
} // namespace raytracer