#pragma once

#include "raytracer.h"
#include "hittable.h"
#include "vec3.h"

class moving_sphere : public hittable {
	public:
		moving_sphere() = default;
		moving_sphere(point3 c0, point3 c1, float r, sPtr<material> m):
			center0(c0), center1(c1), centerVec(c1 - c0), radius(r), mat(m) 
		{
			const auto rvec = vec3(radius, radius, radius);
			const aabb box0 = aabb(center0 - rvec, center0 + rvec);
			const aabb box1 = aabb(center1 - rvec, center1 + rvec);

			bbox = aabb(box0, box1);
		}

		bool hit(const ray& r, interval rayT, hit_record& rec) const override {
			vec3 oc = r.origin() - center(r.time());

			float a = r.direction().lengthSquared();
			float halfB = dot(oc, r.direction());
			float c = oc.lengthSquared() - radius * radius;

			float discrminant = halfB * halfB - a * c;
			if( discrminant < 0 ) return false; 	
			float sqrtd = sqrt(discrminant);

			float root = (-halfB - sqrtd) / a;
			if(!rayT.contains(root)) {
				root = (-halfB + sqrtd) / a;
				if(!rayT.contains(root)) return false;
			}

			rec.t = root;
			rec.p = r.at(rec.t);
			vec3 outwardNormal = (rec.p - center(r.time())) / radius;
			rec.setFaceNormal(r, outwardNormal);
			rec.mat = mat;

			return true;
		}

		point3 center(float time) const {
			// Lineraly interpolate between center0 and center1 using centerVec;
			return center0 + time * centerVec;
		}

		virtual aabb bounding_box() const override { return bbox; }

	public:
		point3 center0, center1;
		vec3 centerVec;
		float radius;
		sPtr<material> mat;
		aabb bbox;
};
