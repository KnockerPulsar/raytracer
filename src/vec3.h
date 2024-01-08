#pragma once

#include <cmath>
#include <iostream>
#include <tuple>

#include "random.h"

using std::sqrt, std::min, std::max, std::tuple;
#define u8 uint8_t

class vec3 {
	public: 
		float e[3];


		vec3(): e{0, 0, 0} {}
		vec3(float e0, float e1, float e2): e{e0, e1, e2} {}

		float x() const { return e[0]; }
		float y() const { return e[1]; }
		float z() const { return e[2]; }

		vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
		float operator[](int i) const { return e[i]; }
		float& operator[](int i) { return e[i]; }

		vec3& operator+=(const vec3& v) {
			e[0] += v.e[0];
			e[1] += v.e[1];
			e[2] += v.e[2];

			return *this;
		}

		vec3& operator*=(float t) {
			e[0] *= t;
			e[1] *= t;
			e[2] *= t;
			
			return *this;
		}

		vec3& operator/=(float t) {
			return *this *= 1/t;
		}

		float lengthSquared() const {
			return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
		}

		float length() const {
			return sqrt(lengthSquared());
		}
		
		static vec3 random() {
			return vec3(randomFloat(), randomFloat(), randomFloat());
		}

		static vec3 random(float min, float max) {
			return vec3(randomFloat(min, max), randomFloat(min, max), randomFloat(min, max));
		}

		bool nearZero() const {
			const float s = 1e-8;
			return (
				std::fabs(e[0]) < s &&
				std::fabs(e[1]) < s &&
				std::fabs(e[2]) < s
			);
		}
};

using point3 = vec3;
using color = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3 &v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(float t, const vec3& v) {
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, float t) {
	return t * v;
}

inline vec3 operator/(const vec3& v, float t) {
	return (1/t) * v;
}

inline float dot(const vec3& u, const vec3& v) {
	return u.e[0] * v.e[0] + 
           u.e[1] * v.e[1] + 
           u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(
        u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]
    );
}

inline vec3 unitVector(vec3 v) {
	return v / v.length();
}

vec3 randomInUnitSphere() {
	while(true) {
		auto p = vec3::random(-1, 1);
		if(p.lengthSquared() >= 1) continue;

		return p;
	}
}

vec3 randomUnitVector() {
	return unitVector(randomInUnitSphere());
}

vec3 randomInHemisphere(const vec3& normal) {
	vec3 inUnitSphere = randomInUnitSphere();
	if(dot(inUnitSphere, normal) > 0) 
		return inUnitSphere;

	return -inUnitSphere;
}

vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3& uv, const vec3& n, float etaIOverEtaT) {
	float cosTheta = fmin(dot(-uv, n), 1);
	vec3 rOutPerp = etaIOverEtaT * (uv + cosTheta * n);
	vec3 rOutParallel = -sqrt(std::fabs(1 - rOutPerp.lengthSquared())) * n;

	return rOutPerp + rOutParallel;
}

vec3 randomInUnitDisk() {
	while(true) {
		vec3 p = vec3(randomFloat(-1, 1), randomFloat(-1, 1), 0);
		if(p.lengthSquared() >= 1) continue;
		return p;
	}
}
