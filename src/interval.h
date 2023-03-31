#pragma once

#include "raytracer.h"
#include <memory>

class interval {
	public:
		float min, max;

		interval(): min(+infinity), max(-infinity) {}
		interval(float _min, float _max): min(_min), max(_max) {}
		interval(const interval& a, const interval& b)
			: min(std::min(a.min, b.min)), max(std::max(a.max, b.max)) {}

		bool contains(float x) const {
			return min <= x && x <= max;
		}

		float clamp(float x) const {
			if(x < min) return min;
			if(x > max) return max;
			return x;
		}

		float size() const {
			return max - min;
		}

		interval expand(float delta) const {
			const auto padding = delta / 2;
			return interval(min - padding, max + padding);
		}
		
		static const interval empty,universe; 
};

const static interval empty   (+infinity, -infinity);
const static interval universe(-infinity, +infinity);
