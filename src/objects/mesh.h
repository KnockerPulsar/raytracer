#pragma once

#include "common/Raytracable.h"

namespace rt::raytracer {
  struct Model {
    struct Triangle {
      vec3 v0, v1, v2, normal;
    };

    Model(std::shared_ptr<::Model> const &model);
    void transform(Transformation const &transformation);

    std::vector<Triangle> triangles;
  };

  class Mesh : public Model, public rt::Raytracable {
  public:
    Mesh(std::shared_ptr<::Model> const &model, Transformation const &transformation,
         sPtr<raytracer::Material> const &material);
    ~Mesh();

    bool Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const override;

    class MeshBVHNode;
    std::unique_ptr<MeshBVHNode> bvhRoot;
  };
} // namespace rt::raytracer
