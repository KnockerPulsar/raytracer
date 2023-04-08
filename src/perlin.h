#pragma once

#include <cmath>

#include "vec3.h"

class perlin {
	public:
		perlin() {
			ranvec = new vec3[point_count];	
			for (int i = 0; i < point_count; ++i) {
				ranvec[i] = unitVector(vec3::random(-1, 1));
			}

			perm_x = perlin_generate_perm();
			perm_y = perlin_generate_perm();
			perm_z = perlin_generate_perm();
		}

		~perlin() { 
			delete[] ranvec;
			delete[] perm_x;
			delete[] perm_y;
			delete[] perm_z;
		}

		float noise(const point3& p) const {
			auto u = p.x() - std::floor(p.x());
			auto v = p.y() - std::floor(p.y());
			auto w = p.z() - std::floor(p.z());

			auto i = static_cast<int>(std::floor(p.x()));
			auto j = static_cast<int>(std::floor(p.y()));
			auto k = static_cast<int>(std::floor(p.z()));
			vec3 c[2][2][2];

			for (int di = 0; di < 2; di++)
				for (int dj = 0; dj < 2; dj++)
					for (int dk = 0; dk < 2; dk++)
						// Sample from the neighbouring 8 vertices with wrap around 
						// (to avoid out of bounds accesses)
						c[di][dj][dk] = ranvec[
							perm_x[(i+di) & 255] ^ 
							perm_y[(j+dj) & 255] ^ 
							perm_z[(k+dk) & 255]  
						];


			return perlin_interp(c, u, v, w);
		}

		float turb(const point3& p, int depth=7) const {
			auto accum = 0.f;
			auto temp_p = p;
			auto weight = 1.f;
		
			for (int i = 0; i < depth; i++) {
				accum += weight * noise(temp_p);
				weight *= 0.5;
				temp_p *= 2;
			}

			return fabs(accum);
		}

	private:
		static const int point_count = 256;
		vec3* ranvec;
		int *perm_x, *perm_y, *perm_z;

		static int* perlin_generate_perm() {
			auto p = new int[point_count];

			for (int i = 0; i < perlin::point_count; i++)
				p[i] = i;

			permute(p, point_count);

			return p;
		}

		static void permute(int* p, int n) {
			for (int i = n-1; i > 0; i--) {
				int target = randomInt(0, i);
				int tmp = p[i];
				p[i] = p[target];
				p[target] = tmp;
			}
		}

		static float perlin_interp(vec3 c[2][2][2], float u, float v, float w) {
			auto accum = 0.f;
	
			auto uu = u * u * (3 - 2*u);
			auto vv = v * v * (3 - 2*v);
			auto ww = w * w * (3 - 2*w);

			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int k = 0; k < 2; k++) {
						// x * t +  y * (1-t) is a linear interpolation from x to y using t.
						// i, j, k = 0, 0, 0
						// accum += 
						// 	* (1 * (1-u))
						// 	* (1 * (1-v))
						// 	* (1 * (1-w))
						// 	* c[0][0][0]
						//
						// i, j, k = 0, 0, 1
						// accum += 
						// 	*	(1 * (1-u))
						// 	*	(1 * (1-v))
						// 	*	(w)
						// 	*	c[0][0][1]
						vec3 weight_v(u-i, v-j, w-k);

						accum += 
								(i*uu + (1-i)*(1-uu)) 
							* (j*vv + (1-j)*(1-vv))
							* (k*ww + (1-k)*(1-ww))
							* dot(c[i][j][k], weight_v);
					}
			return accum;
		}
};
