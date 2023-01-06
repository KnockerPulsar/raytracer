#pragma once

#include "hittable.h"
#include "interval.h"
#include "raytracer.h"
#include <vector>

class hittable_list : public hittable {
	public:
		std::vector<sPtr<hittable>> objects;


		hittable_list() = default;
		hittable_list(sPtr<hittable> object) { add(object); }

		void clear() { objects.clear(); }
		void add(sPtr<hittable> object) { objects.push_back(object); }

		virtual bool hit(const ray& r, interval rayT, hit_record& rec) const override {
			hit_record tempRec;
			bool hitAnything = false;
			float closestSoFar = rayT.max;

			for(const auto& object : objects) {
				if(object->hit(r, interval(rayT.min, closestSoFar), tempRec)) {
					hitAnything = true;
					closestSoFar = tempRec.t;
					rec = tempRec;
				}
			}

			return hitAnything;
		}
};
