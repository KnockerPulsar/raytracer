#pragma once

#include "Defs.h"
#include "Hittable.h"
#include "acceleration_structure.h"

#include <memory>
#include <optional>
#include <vector>

using ::std::make_shared;
using ::std::shared_ptr, std::optional, std::make_optional;

namespace rt::raytracer {
  class HittableList : public AccelerationStructure {
  public:
    HittableList(std::vector<std::shared_ptr<Hittable>> &&objects) : objects{std::move(objects)} {}

    bool hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override;
  private:
    std::vector<sPtr<Hittable>> objects;
  };
} // namespace rt
