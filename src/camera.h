#pragma once

#include "raytracer.h"
#include "vec3.h"

class camera {
	public:
		// Vertical camera FOV in degrees
		float vFov 			= 40;
		float aperture  = 0;
		float focusDist = 10;

		point3 lookfrom = point3(0, 0, -1);
		point3 lookat   = point3(0, 0, 0);
		vec3 	 vup			= vec3(0, 1, 0);


		void initialize(float aspectRatio = 1.0f) {
			float theta = degreesToRadians(vFov);
			float h = tan(theta / 2.0);

			float viewportHeight = 2 * h;
			float viewportWidth = aspectRatio * viewportHeight;

			w = unitVector(lookfrom - lookat);
			u = unitVector(cross(vup, w));
			v = cross(w, u);

			origin 					= lookfrom;
			horizontal 			= focusDist * viewportWidth * u;
			vertical 				= focusDist * viewportHeight * v;
			lowerLeftCorner = origin - horizontal/2 - vertical/2 - focusDist * w;

			lensRadius = aperture/2;
		}

		ray getRay(float s, float t) const {
			// s, t are in normalized image coordinates (like uvs)
			// s: 0->1, left -> right
			// t: 0->1, top  -> bottom

			vec3 rd = lensRadius * randomInUnitDisk();
			vec3 offset = u * rd.x() + v * rd.y();

			return ray(
					origin + offset, 
					lowerLeftCorner + s * horizontal + (1-t) * vertical - origin - offset
			);
		}

	private:
		vec3 origin;				 
		vec3 horizontal;
		vec3 vertical;
		vec3 lowerLeftCorner;
		vec3 	 u, v, w;
		float lensRadius;
};
