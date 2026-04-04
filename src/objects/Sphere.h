#pragma once
#include "common/Defs.h"
#include "common/Raytracable.h"
#include "data_structures/vec3.h"

namespace rt::raytracer {
  class Material;

  class Sphere : public Raytracable {
  public:
    Sphere(float r, Transformation const &transformation, sPtr<raytracer::Material> const &material);

    bool Hit(const rt::Ray &worldspaceRay, float t_min, float t_max, rt::HitRecord &rec) const override;

    // json toJsonSpecific() const override;
    //
    // void Rasterize(vec3 color) override;

  private:
    static void GetSphereUV(const vec3 &p, float &u, float &v);

    float const radius;
    vec3 const center;
  };

  // inline void from_json(const json &j, Sphere &s) {
  //   s.transformation = j["transform"].get<Transformation>();
  //   s.radius         = j["radius"].get<float>();
  //   s.material       = MaterialFactory::FromJson(j["material"]);
  // }
  //
  // inline void to_json(json &j, const Sphere &s) { j = s.toJson(); }
  //
} // namespace rt
