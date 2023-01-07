#pragma once

#include "raytracer.h"

class camera {
	public:
		void initialize(float aspectRatio = 1.0f) {
			float viewportHeight = 2.0f;
			float viewportWidth = aspectRatio * viewportHeight;
			float focalLength = 1.0f;


			origin 					= point3(0., 0., 0.);
			horizontal 			= vec3(viewportWidth, 0, 0);
			vertical 				= vec3(0, viewportHeight, 0);
			lowerLeftCorner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focalLength);
		}

		ray getRay(float s, float t) const {
			// s, t are in normalized image coordinates (like uvs)
			// s: 0->1, left -> right
			// t: 0->1, top  -> bottom

			return ray(origin, lowerLeftCorner + s * horizontal + (1-t) * vertical - origin);
		}

	private:
		vec3 origin;				 
		vec3 horizontal;
		vec3 vertical;
		vec3 lowerLeftCorner;
};
