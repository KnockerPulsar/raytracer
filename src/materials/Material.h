#pragma once

#include "data_structures/vec3.h"

namespace rt {
  class Ray;
  class HitRecord;
  namespace raytracer {
    struct HitRecord;

    class Material {
    public:
      virtual vec3 emitted(float u, float v, const vec3 &p) const { return vec3::Zero(); }
      virtual bool scatter(const rt::Ray &r_in, rt::raytracer::HitRecord &rec, vec3 &attenuation, Ray &scattered) const = 0;
      virtual json toJson() const                                                                            = 0;
    };
  } // namespace raytracer
} // namespace rt
