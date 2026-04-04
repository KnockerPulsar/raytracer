#pragma once

#include "Hittable.h"
#include "Transformation.h"

namespace rt
{
  class Raytracable: public Hittable {
  public:
    Raytracable(Transformation const &transformation, sPtr<raytracer::Material> const &material,
                AABB const &boundingBox)
        : Hittable(boundingBox), transformation(transformation), material(material) {}
    virtual ~Raytracable() = default;

    Transformation const            transformation;
    sPtr<raytracer::Material> const material;
  };
}
