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
}; // namespace raytracer
