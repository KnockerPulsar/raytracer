#pragma once

#include "interval.h"
#include "ray.h"
#include "raytracer.h"


class material;

struct hit_record {
	point3 p;
	vec3 normal;
	sPtr<material> mat;
	float t;
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
};
