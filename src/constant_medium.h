#pragma once

#include "hittable.h"
#include "interval.h"
#include "material.h"
#include "random.h"
#include "raytracer.h"
#include "texture.h"
#include <memory>

class constant_medium: public hittable {
	public:
		std::shared_ptr<hittable> boundary;
		float neg_inv_density;
		std::shared_ptr<material> phase_function;

	public:
		constant_medium(std::shared_ptr<hittable> b, float d, std::shared_ptr<texture> a)
			: boundary(b), neg_inv_density(-1/d), phase_function(std::make_shared<isotropic>(a))
		{}

		constant_medium(std::shared_ptr<hittable> b, float d, color c)
			: boundary(b), neg_inv_density(-1/d), phase_function(std::make_shared<isotropic>(c))
		{}

		
		bool hit(const ray&r, interval ray_t, hit_record& rec) const override {
			const bool enable_debug = false;
			const bool debugging = enable_debug && randomFloat() < 1e-5;

			hit_record rec1, rec2;

			// Check if this ray hits the boundary at all
			if (!boundary->hit(r, interval(-infinity, infinity), rec1))
				return false;

			// Check if we hit the boundary from inside
			// the `interval(rec1.t + 0.0001, infinity)` is meant to make it so 
			// that the ray starts inside the boundary (at the other side of the hit),
			// 		check 1		|				check 2				|																		
			// 			------->+ ------------------->+-------------> (to infinity)
			// 							|	^ rec1.t + 0.0001		|										
			// 							|							 				|																						
			// 							/----inside volume----/
			if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))
				return false;

			if (debugging) std::clog << "\nray_tmin=" << rec1.t << ", ray_tmax" << rec2.t << "\n";

			if (rec1.t < ray_t.min) rec1.t = ray_t.min;
			if (rec2.t > ray_t.max) rec2.t = ray_t.max;

			if (rec1.t >= rec2.t)
				return false;

			if (rec1.t < 0)
				rec1.t = 0;

			const auto ray_length = r.direction().length();
			const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
			const auto hit_distance = neg_inv_density * log(randomFloat());

			if (hit_distance > distance_inside_boundary)
				return false;

			rec.t = rec1.t + hit_distance / ray_length;
			rec.p = r.at(rec.t);

			if (debugging) {
				std::clog << "hit_distance = " << hit_distance << "\n"
									<< "rec.t = " << rec.t << "\n"
									<< "rec.p = " << rec.p << "\n";
			}

			rec.normal = vec3(1, 0, 0); // Arbitrary
			rec.frontFace = true;			// Also arbitrary
			rec.mat = phase_function;

			return true;

		}

    aabb bounding_box() const override { return boundary->bounding_box(); }
};
