#pragma once 

#include <cmath>
#include <random>

inline float randomFloat() {
	/* return rand() / (float(RAND_MAX) + 1.0f); */
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline float randomFloat(float min, float max) {
	return min + (max-min)  * randomFloat();
}

inline int randomInt(int min, int max) {
		return static_cast<int>(randomFloat(min, max+1));
}
