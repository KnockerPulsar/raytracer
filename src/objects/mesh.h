#pragma once

#include "Hittable.h"

namespace rt {
  class Mesh: public Hittable {
    public:
      Mesh(std::filesystem::path path);
      ~Mesh();

      bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;
      bool BoundingBox(float t0, float t1, AABB &outputBox) const override;
      void Rasterize(vec3 color) override;

    private:
      struct Triangle {
        vec3 v0, v1, v2, normal;
      };

      static auto load_triangles(Model model) -> std::vector<Triangle>;
      // TODO this is duplicated from `rt::Triangle`, both should be using the same code
      static bool triangle_hit(Triangle const &tri, Mesh const *hittable, const Ray &r, float t_min, float t_max,
                               HitRecord &rec);

      Model const model; // Useful for editor visualizations
      // TODO probably can be better optimized?
      std::vector<Triangle> const triangles;
      AABB mutable local_space_bb; // `mutable` is a hack...
  };
}
