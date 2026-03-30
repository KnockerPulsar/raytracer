#pragma once

namespace rt {
  class Ray;
  struct HitRecord;

  namespace raytracer {
    class AccelerationStructure {
    public:
      virtual ~AccelerationStructure() = default;

      // common
      virtual bool hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const = 0;
    };
  } // namespace raytracer
} // namespace rt
