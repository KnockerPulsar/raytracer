#pragma once

#include "raytracer.h"
#include "interval.h"

class aabb {
	public:
		aabb() {}

		aabb(const interval& ix, const interval& iy, const interval& iz)
			: x(ix), y(iy), z(iz) {}

		aabb(const aabb& box0, const aabb& box1) {
			x = interval(box0.x, box1.x);
			y = interval(box0.y, box1.y);
			z = interval(box0.z, box1.z);
		}

		aabb(const point3&a, const point3& b) {
			x = interval(min(a[0], b[0]), max(a[0], b[0]));	
			y = interval(min(a[1], b[1]), max(a[1], b[1]));	
			z = interval(min(a[2], b[2]), max(a[2], b[2]));	
		}

		const interval& axis(int n) const {
			if (n == 1) return y;
			if (n == 2) return z;

			return x;
		}

		bool hit(const ray& r, interval ray_t) const {
			for (int a = 0; a < 3; a++) {
				float invD = 1.0f / r.direction()[a];

				float t0 = axis(a).min - r.origin()[a] * invD;
				float t1 = axis(a).max - r.origin()[a] * invD;

				if (invD < 0.0f) 
					std::swap(t0, t1);

				float ray_tmin = t0 > ray_t.min? t0 : ray_t.min;
				float ray_tmax = t1 < ray_t.max? t1 : ray_t.max;

				if (ray_tmax <= ray_tmin)
					return false;
			}
			return true;
		}

	public:
			interval x, y, z;
};
