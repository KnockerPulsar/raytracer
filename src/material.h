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
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

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

class dielectric : public material {
  public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        attenuation = color(1.0, 1.0, 1.0);

        // 1.0 is the refraction index for air
        // Front face means going into the object, eta_i is 1.0 (air), eta_t is the object's ir.
        // Back face is the opposite, (eta_t / 1.0) = (ir / 1.0) = ir
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3   unit_direction = unit_vector(r_in.direction());
        double cos_theta      = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta      = sqrt(1 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);
        return true;
    }

  private:
    double ir;

    static double reflectance(double cosine, double ref_idx) {
        // Shlick's approximation
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0      = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};
