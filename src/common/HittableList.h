#pragma once

#include "Raytracable.h"
#include "acceleration_structure.h"

#include <memory>
#include <vector>

namespace rt::raytracer {
  class HittableList : public AccelerationStructure {
  public:
    HittableList(std::vector<std::unique_ptr<Raytracable>> &&objects) : objects{std::move(objects)} {}

    bool hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override;
  private:
    std::vector<std::unique_ptr<rt::Raytracable>> objects;
  };
} // namespace rt
