#pragma once
#include "aabb.h"
#include "hittable.h"
#include "vec3.h"
#include <algorithm>
#include <cmath>

class sphere: public hittable {
	public:
		sphere() {}
		sphere(point3 ctr, float r, sPtr<material> m): center(ctr), radius(r), mat(m) {
			const auto rvec = vec3(radius, radius, radius);
			bbox = aabb(center-rvec, center+rvec);
		};

		
		virtual bool hit(const ray& r, interval rayT, hit_record& rec) const override {
			vec3 oc = r.origin() - center;

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
			vec3 outwardNormal = (rec.p - center) / radius;
			rec.setFaceNormal(r, outwardNormal);

			get_sphere_uv(outwardNormal, rec.u, rec.v);
			rec.mat = mat;

			return true;
		}

		virtual aabb bounding_box() const override { return bbox; }

	public:
		point3 center;
		float radius;
		sPtr<material> mat;
		aabb bbox;

	private:
		static void get_sphere_uv(const point3& p, float& u, float& v) {
			// p: a given point on the sphere of radius one, centered at the origin.
			// u: returned value [0,1] of angle around the Y axis from X=-1.
			// v: returned value [0,1] of angle from Y=-1 to Y=+1.
			//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
			//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
			//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

			auto phi = std::atan2(-p.z(), p.x()) + pi;
			auto theta = std::acos(-p.y());

			u = phi / (2*pi);
			v = theta / pi;
		}
};
