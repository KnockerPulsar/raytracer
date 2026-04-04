#pragma once

#include "Hittable.h"
#include "Transformation.h"

namespace rt
{
  class Raytracable: public Hittable {
  public:
    Raytracable(Transformation const &transformation, sPtr<raytracer::Material> const &material,
                AABB const &localspaceBoundingBox)
        : Hittable(AABB::ToWorldspace(localspaceBoundingBox, transformation)), transformation(transformation),
          material(material) {}

    virtual ~Raytracable() = default;

    Transformation const            transformation;
    sPtr<raytracer::Material> const material;
  };
}
