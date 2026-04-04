#pragma once

#include "common/Raytracable.h"

namespace rt::raytracer {
  struct Model {
    struct Triangle {
      vec3 v0, v1, v2, normal;
    };

    Model(std::shared_ptr<::Model> const& model);

    std::vector<Triangle> const    triangles;
  };

  class Mesh : public Model, public rt::Raytracable {
  public:
    Mesh(std::shared_ptr<::Model> const &model, Transformation const &transformation,
         sPtr<raytracer::Material> const &material);

    bool Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const override;
  };
} // namespace rt::raytracer
