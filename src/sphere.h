#pragma once
#include "hittable.h"
#include "vec3.h"
#include <cmath>

class sphere: public hittable {
	public:
		point3 center;
		float radius;
		sPtr<material> mat;


		sphere() {}
		sphere(point3 ctr, float r, sPtr<material> m): center(ctr), radius(r), mat(m) {};

		
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
			rec.mat = mat;

			return true;
		}
};
