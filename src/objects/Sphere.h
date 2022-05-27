#pragma once
#include "../Defs.h"
#include "../Hittable.h"
#include "../data_structures/vec3.h"
#include "../materials/MaterialFactory.h"

namespace rt {
  class Material;

  class Sphere : public Hittable {
  public:
    float          radius;
    vec3           center;
    sPtr<Material> mat_ptr;

    Sphere() = default;
    Sphere(float r, vec3 pos, sPtr<Material> m);

    bool Hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const override;

    bool BoundingBox(float t0, float t1, AABB &outputBox) const override;

    json GetJsonDerived() const override;

    void Rasterize() override;

    void OnImgui() override;

  private:
    static void GetSphereUV(const vec3 &p, float &u, float &v);
  };

  inline void from_json(const json &j, Sphere &s) {
    s.center         = j["pos"].get<vec3>();
    s.radius         = j["radius"].get<float>();
    s.mat_ptr        = MaterialFactory::FromJson(j["material"]);
    s.transformation = j["transform"].get<Transformation>();
  }

  inline void to_json(json &j, const Sphere &s) { j = s.GetJson(); }

} // namespace rt