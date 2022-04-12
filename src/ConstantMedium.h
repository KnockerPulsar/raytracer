#pragma once

#include "Constants.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "Isotropic.h"
#include "Util.h"
#include "textures/Texture.h"
#include <iostream>
#include <memory>
#include <raylib.h>

namespace rt {

  // Works for convex boundries only
  class ConstantMedium : public Hittable {
  public:
    sPtr<Hittable> boundry;
    sPtr<Material> phaseFunction;
    double         negInvDensity;

    ConstantMedium(sPtr<Hittable> _boundry, double _density, sPtr<Texture> _tex)
        : boundry(_boundry), negInvDensity(-1 / _density),
          phaseFunction(make_shared<Isotropic>(_tex)) {}

    ConstantMedium(sPtr<Hittable> _boundry, double _density, vec3 color)
        : boundry(_boundry), negInvDensity(-1 / _density),
          phaseFunction(make_shared<Isotropic>(color)) {}

    virtual bool Hit(const Ray &r, float t_min, float t_max,
                     HitRecord &rec) const override;

    virtual bool BoundingBox(float t0, float t1,
                             AABB &outputBox) const override {
      return boundry->BoundingBox(t0, t1, outputBox);
    }
  };

  bool ConstantMedium::Hit(const Ray &r, float t_min, float t_max,
                           HitRecord &rec) const {

    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging   = enableDebug && RandomFloat() < 0.00001;

    HitRecord rec1, rec2;

    if (!boundry->Hit(r, -infinity, infinity, rec1)) {
      return false;
    }

    if (!boundry->Hit(r, rec1.t + 0.0001, infinity, rec2)) {
      return false;
    }

    if (debugging)
      std::cerr << "\nt_min" << rec1.t << ", t_max" << rec2.t << "\n";

    if (rec1.t < t_min)
      rec1.t = t_min;
    if (rec2.t > t_max)
      rec2.t = t_max;

    if (rec1.t >= rec2.t)
      return false;

    if (rec1.t < 0)
      rec1.t = 0;

    const auto rayLength         = r.direction.Len();
    const auto distInsideBoundry = (rec2.t - rec1.t) * rayLength;
    const auto hitDist           = negInvDensity * log(RandomFloat());

    if (hitDist > distInsideBoundry)
      return false;

    rec.t = rec1.t + hitDist / rayLength;
    rec.p = r.At(rec.t);

    if (debugging) {
      std::cerr << "hit_distance = " << hitDist << '\n'
                << "rec.t = " << rec.t << '\n'
                << "rec.p = "
                << "{" << rec.p.x << "," << rec.p.y << "," << rec.p.z << "}"
                << '\n';
    }

    rec.normal     = vec3(1, 0, 0); // Aribtrary
    rec.front_face = true;          // Aribtrary
    rec.mat_ptr    = phaseFunction;

    return true;
  }
} // namespace rt