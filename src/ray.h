#pragma once

#include "vec3.h"

class ray {
	public:
		point3 orig;
		vec3 dir;
		float tm;


		ray() = default;
		ray(const point3& origin, const vec3& direction, float time = 0) 
			: orig(origin), dir(direction), tm(time) {}

		point3 origin() const { return orig; }
		point3 direction() const { return dir; }
		float time() const { return tm; }

		point3 at(float t) const {
			return orig + t * dir;
		}
};
