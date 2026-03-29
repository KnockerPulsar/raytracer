#pragma once

#include <memory>
#include <vector>

namespace rt {
  class Ray;
  struct Hittable;

  namespace raytracer {

    struct HitRecord;
    class AccelerationStructure {
    public:
      virtual ~AccelerationStructure() = default;

      // common
      virtual bool hit(const rt::Ray &r, float t_min, float t_max, rt::raytracer::HitRecord &rec) const = 0;

      // editor only
      virtual void add(std::shared_ptr<Hittable> new_child)                        = 0;
      virtual void remove(std::shared_ptr<Hittable> new_child)                     = 0;
      virtual auto get_hittables() const -> std::vector<std::shared_ptr<Hittable>> = 0;
      virtual void regenerate()                                                    = 0;
    };
  } // namespace raytracer
} // namespace rt
