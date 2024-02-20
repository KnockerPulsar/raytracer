#pragma once

#include "color.h"
#include "hittable.h"
#include "vec3.h"

class hit_record;

class material {
  public:
    virtual ~material() = default;
    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const = 0;
};

class lambertian : public material {
  public:
    lambertian(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        vec3 scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered   = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

class metal : public material {
  public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1? f: 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered      = ray(rec.p, reflected + fuzz * random_unit_vector());
        attenuation    = albedo;

        return dot(scattered.direction(), rec.normal) > 0;
    }

  private:
    color  albedo;
    double fuzz;
};
