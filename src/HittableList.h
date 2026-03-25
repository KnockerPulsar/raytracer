#pragma once

#include "Defs.h"
#include "Hittable.h"
#include "acceleration_structure.h"

#include <memory>
#include <optional>
#include <vector>

using ::std::make_shared;
using ::std::shared_ptr, std::optional, std::make_optional;

namespace rt {
  class HittableList : public AccelerationStructure {
  public:
    HittableList(sPtr<Hittable> object) : objects{{object}} {}
    HittableList(std::vector<std::shared_ptr<Hittable>> &&objects) : objects{std::move(objects)} {}

    bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
    void add(std::shared_ptr<Hittable> new_child) override;
    void remove(std::shared_ptr<Hittable> child_to_remove) override;
    auto get_hittables() const -> std::vector<std::shared_ptr<Hittable>> override;
    void regenerate() override;

  private:
    std::vector<sPtr<Hittable>> objects;
  };
} // namespace rt
