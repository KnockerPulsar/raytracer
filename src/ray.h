#pragma once

#include "vec3.h"

class ray {
	public:
		point3 orig;
		vec3 dir;


		ray() = default;
		ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}
		
		point3 origin() const { return orig; }
		point3 direction() const { return dir; }

		point3 at(float t) const {
			return orig + t * dir;
		}
};
