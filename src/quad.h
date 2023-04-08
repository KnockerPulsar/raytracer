#pragma once

#include "hittable.h"
#include "hittable_list.h"
#include <memory>
class quad : public hittable {
	public:
		quad(const point3& _Q, const vec3& _u, const vec3& _v, std::shared_ptr<material> m)
			: Q(_Q), u(_u), v(_v), mat(m) 
		{
			auto n = cross(u, v);
			normal = unitVector(n);
			D = dot(normal, Q);
			w = n / dot(n, n);

			set_bounding_box();
		}

		virtual void set_bounding_box() {
			bbox = aabb(Q, Q + u + v).pad();
		}

		aabb bounding_box() const override { return bbox; }

		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			auto denom = dot(normal, r.direction());

			// No hit if the ray is parallel to the plane.
			if (fabs(denom) < 1e-8)
				return false;

			auto t = (D - dot(normal, r.origin())) / denom;
			
			// Outside the ray interval.
			if(!ray_t.contains(t))
				return false; 

			auto intersection = r.at(t);
			vec3 planar_hitpt_vector = intersection - Q;
			auto alpha = dot(w, cross(planar_hitpt_vector, v));
			auto beta = dot(w, cross(u, planar_hitpt_vector));

			if (!is_interior(alpha, beta, rec))
				return false;

			rec.t = t;
			rec.p = intersection;
			rec.mat = mat;
			rec.setFaceNormal(r, normal);

			return true;
		}

		virtual bool is_interior(float a, float b, hit_record& rec) const {
			if ((a<0) || (1<a) || (b<0) || (1<b))
				return false;

			rec.u = a;
			rec.v = b;

			return true;
		}

	private:
		point3 Q;
		vec3 u, v, w, normal;
		std::shared_ptr<material> mat;
		aabb bbox;
		float D;
};

inline std::shared_ptr<hittable_list> box(const point3& a, const point3& b, std::shared_ptr<material> mat) {
	auto sides = std::make_shared<hittable_list>();

	auto min = point3(fminf(a.x(), b.x()), fminf(a.y(), b.y()), fminf(a.z(), b.z()));
	auto max = point3(fmaxf(a.x(), b.x()), fmaxf(a.y(), b.y()), fmaxf(a.z(), b.z()));

	auto dx = vec3(max.x() - min.x(), 0, 0);
	auto dy = vec3(0, max.y() - min.y(), 0);
	auto dz = vec3(0, 0, max.z() - min.z());

	sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
	sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
	sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
	sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

	return sides;
}
