#pragma once

#include "hittable.h"

class sphere : public hittable {
  public:
    sphere(point3 _center, double _radius, shared_ptr<material> _material)
        : center1(_center), radius(_radius), mat(_material), is_moving(false) {}

    sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material)
        : center1(_center1), radius(_radius), mat(_material), is_moving(true) {
		center_vec = _center2 - _center1;
	}

    bool hit(const ray& r_in, interval ray_t, hit_record& rec) const override {
	point3 center = is_moving? sphere_center(r_in.time()): center1;
        vec3 oc     = r_in.origin() - center;
        auto a      = r_in.direction().length_squared();
        auto half_b = dot(r_in.direction(), oc);
        auto c      = oc.length_squared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root)) return false;
        }

        rec.t               = root;
        rec.p               = r_in.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r_in, outward_normal);
        rec.mat = mat;

        return true;
    }

  private:
    point3               center1;
    double               radius;
    shared_ptr<material> mat;
    bool                 is_moving;
    vec3                 center_vec;

    point3 sphere_center(double time) const { return center1 + center_vec * time; }
};
