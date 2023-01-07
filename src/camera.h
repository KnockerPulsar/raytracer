#pragma once

#include "raytracer.h"
#include "vec3.h"

class camera {
	public:
		// Vertical camera FOV in degrees
		float vFov = 40;
		point3 lookfrom = point3(0, 0, -1);
		point3 lookat   = point3(0, 0, 0);
		vec3 	 vup			= vec3(0, 1, 0);


		void initialize(float aspectRatio = 1.0f) {
			float theta = degreesToRadians(vFov);
			float h = tan(theta / 2.0);

			float viewportHeight = 2 * h;
			float viewportWidth = aspectRatio * viewportHeight;

			vec3 w = unitVector(lookfrom - lookat);
			vec3 u = unitVector(cross(vup, w));
			vec3 v = cross(w, u);


			origin 					= lookfrom;
			horizontal 			= viewportWidth * u;
			vertical 				= viewportHeight * v;
			lowerLeftCorner = origin - horizontal/2 - vertical/2 - w;
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
