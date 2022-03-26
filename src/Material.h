#pragma once

#include "Hittable.h"
#include "SolidColor.h"
#include "Texture.h"
#include <memory>

namespace raytracer {

  struct HitRecord;

  class Material {
  public:
    virtual Vec3 emitted(float u, float v, const Vec3 &p) const {
      return Vec3::Zero();
    }
    
    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const = 0;
  };

  
  class DiffuseLight : public Material {
  public:
    shared_ptr<Texture> emit;

    DiffuseLight(shared_ptr<Texture> a) : emit(a) {}
    DiffuseLight(Vec3 c) : emit(std::make_shared<SolidColor>(c)) {}

    virtual bool scatter(const Ray &r_in, HitRecord &rec, Vec3 &attenuation,
                         Ray &scattered) const override {
      return false;
    }

    virtual Vec3 emitted(float u, float v, const Vec3 &p) const override {
      return emit->Value(u, v, p);
    }
  };
}; // namespace raytracer
