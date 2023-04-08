#pragma once

#include "interval.h"
#include "ray.h"
#include "raytracer.h"
#include "aabb.h"
#include <memory>

class material;

struct hit_record {
	point3 p;
	vec3 normal;
	sPtr<material> mat;
	float t, u, v;
	bool frontFace;

	void setFaceNormal(const ray& r, const vec3& outwardNormal) {
		frontFace = dot(r.direction(), outwardNormal) < 0;
		normal = frontFace? outwardNormal : -outwardNormal;
	}
};

class hittable {
	public:
		virtual ~hittable() = default;
		virtual bool hit(const ray& r, interval rayT, hit_record& rec) const = 0;
		virtual aabb bounding_box() const = 0;
};

class translate: public hittable {
	public:
		std::shared_ptr<hittable> object;
		vec3 offset;
		aabb bbox;

	public:
		translate(std::shared_ptr<hittable> p, const vec3& displacement)
			: object(p), offset(displacement)
		{
			bbox = object->bounding_box() + offset;
		}

		bool hit(const ray& r, interval rayT, hit_record& rec) const override {
			ray offset_r(r.origin() - offset, r.direction(), r.time());

			if(!object->hit(offset_r, rayT, rec))
				return false;

			rec.p += offset;

			return true;
		}

    aabb bounding_box() const override { return bbox; }
};

class rotate_y: public hittable {
	public:
		std::shared_ptr<hittable> object;
		float sin_theta, cos_theta;
		aabb bbox;

	public:

		rotate_y(std::shared_ptr<hittable> p, float angle_degrees): object(p) {
			auto radians = degreesToRadians(angle_degrees);

			sin_theta = sinf(radians);
			cos_theta = cosf(radians);
			bbox = object->bounding_box();

			point3 min(infinity, infinity, infinity);
			point3 max(-infinity, -infinity, -infinity);

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						auto x = i * bbox.x.max + (1-i) * bbox.x.min;
						auto y = j * bbox.y.max + (1-j) * bbox.y.min;
						auto z = k * bbox.z.max + (1-k) * bbox.z.min;

						auto newx = cos_theta * x + sin_theta * z;
						auto newz = -sin_theta * x + cos_theta * z;

						vec3 tester(newx, y, newz);
						for (int c = 0; c < 3; c++) {
								min[c] = fminf(min[c], tester[c]);
								max[c] = fmaxf(max[c], tester[c]);
						}
					}
				}
			}


			bbox = aabb(min, max);
		}
		
		bool hit(const ray& r, interval rayT, hit_record& rec) const override {
			auto origin = r.origin();
			auto direction = r.direction();

			// When rotating about the Y axis (world to object space):
			// x' = cos(theta) * x - sin(theta) * z
			// z' = sin(theta) * x + cos(theta) * z
			//
			// When rotating about the Y axis (object to world space AKA the inverse transform):
			// replace every theta with -theta. cos(theta) = cos(-theta), sin(-theta) = -sin(theta)
			// x' = cos(theta) * x + sin(theta) * z
			// z' = -sin(theta) * x + cos(theta) * z
			
			// World to object
			origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
			origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];
	
			direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
			direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

			ray rotated_r(origin, direction, r.time());

			if(!object->hit(rotated_r, rayT, rec))
				return false;


			// Object to world
			auto p = rec.p;
			p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
			p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

			auto normal = rec.normal;
			normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
			normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

			rec.p = p;
			rec.normal = normal;

			return true;
		}

		aabb bounding_box() const override { return bbox; }
};
