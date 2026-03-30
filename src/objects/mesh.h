#pragma once

#include "common/Hittable.h"

namespace rt::raytracer {
  class Mesh: public rt::Hittable {
    public:
      Mesh(std::filesystem::path path);
      ~Mesh();

      bool Hit(const rt::Ray &r, float t_min, float t_max, rt::HitRecord &rec) const override;
      bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    private:
      struct Triangle {
        vec3 v0, v1, v2, normal;
      };

      static auto load_triangles(Model model) -> std::vector<Triangle>;
      // TODO this is duplicated from `rt::Triangle`, both should be using the same code
      static bool triangle_hit(Triangle const &tri, Mesh const *hittable, const rt::Ray &r, float t_min, float t_max,
                               rt::HitRecord &rec);

      Model const model; // Useful for editor visualizations
      // TODO probably can be better optimized?
      std::vector<Triangle> const triangles;
      AABB mutable local_space_bb; // `mutable` is a hack...
  };
}
