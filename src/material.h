#pragma once

#include "hittable.h"
#include "raytracer.h"
#include "vec3.h"
#include <cmath>
#include <ctime>
#include <functional>
#include <raylib.h>

struct hit_record;

class material {
	public:
		virtual ~material() = default;

		virtual bool scatter(
			const ray& rIn,
			const hit_record& rec,
			color& attenuation,
			ray& scattered
		) const = 0;
};

class lambertian : public material {
	 public:
			color albedo; 


			lambertian(const color& a) : albedo(a) {}
			virtual bool scatter(
				const ray& rIn,
				const hit_record& rec,
				color& attenuation,
				ray& scattered
			) const override {
				// Can use randomInHemisphere and randomInUnitSphere 
				// for slightly different diffuse looks.
				vec3 scatterDirection = rec.normal + randomUnitVector();

				if(scatterDirection.nearZero()) {
					scatterDirection = rec.normal;
				}

				scattered = ray(rec.p, scatterDirection);
				attenuation = albedo;
				return true;
			}
};

class metal : public material {
	public:
		color albedo;
		float fuzz;

		
		metal(const color& a, float f): albedo(a) {
			if(f > 1) {
				TraceLog(LOG_WARNING, "Fuzz greater than 1 (%.2f), clipping to 1", f);
				fuzz = 1;
			} else {
				fuzz = f;
			}
		}

		virtual bool scatter(
			const ray& rIn,
			const hit_record& rec,
			color& attenuation,
			ray& scattered
		) const override {
			vec3 reflected = reflect(unitVector(rIn.direction()), rec.normal);

			scattered = ray(rec.p, reflected + fuzz * randomInUnitSphere());
			attenuation = albedo;
			return (dot(scattered.direction(), rec.normal) > 0);
		}
};

class dielectric : public material {
	public:
		float ir; // Index of refraction


		dielectric(float indexOfRefraction): ir(indexOfRefraction) {}
		virtual bool scatter(
					const ray& rIn,
					const hit_record& rec,
					color& attenuation,
					ray& scattered
		) const override {
			attenuation = color(1, 1, 1);
			float refractionRatio = rec.frontFace? (1 / ir): ir;

			vec3 unitDirection = unitVector(rIn.direction());
			float cosTheta = fmin(dot(-unitDirection, rec.normal), 1);
			float sinTheta = sqrt(1 - cosTheta * cosTheta);

			bool cannotRefract = refractionRatio * sinTheta > 1; 
			bool schlick = reflectance(cosTheta, refractionRatio) > randomFloat();
			vec3 direction;

			if(cannotRefract || schlick) {
				direction = reflect(unitDirection, rec.normal);
			} else {
				direction = refract(unitDirection, rec.normal, refractionRatio);
			}

			scattered = ray(rec.p, direction);
			return true;
		}

	private:
		static float reflectance(float cosine, float refIndex) {
			// Schlick's approx.
			float r0 = (1 - refIndex) / (1 + refIndex);
			r0 = r0 * r0;
			return r0 + (1-r0)*pow(1-cosine, 5);
		}
};