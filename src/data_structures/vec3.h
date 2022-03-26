#pragma once

#include "../../vendor/nlohmann-json/json.hpp"
#include "../Defs.h"
#include "../Util.h"
#include <raylib.h>
#include <raymath.h>

class vec3 : public Vector3 {
public:
  vec3();
  vec3(const float);
  vec3(const float, const float, const float);
  vec3(Vector3);

  bool NearZero() const;
  vec3 Reflect(const vec3 &norm) const;
  vec3 Refract(const vec3 &n, float etai_over_etat);

  inline vec3  Normalize() const;
  inline float SqrLen() const;
  inline float Len() const;

  inline vec3 operator-() const;
  inline vec3 operator-(const vec3 &) const;
  inline vec3 operator+(const vec3 &) const;
  inline vec3 operator/(float) const;
  inline vec3 operator*(const vec3 &) const;

  inline vec3 &operator+=(const vec3 &);
  inline vec3 &operator/=(float);
  inline vec3 &operator*=(float);

  static float DotProd(const vec3 &, const vec3 &);
  static vec3  CrsProd(const vec3 &, const vec3 &);

  static vec3 Random();
  static vec3 Random(float min, float max);
  static vec3 RandomInUnitSphere();
  static vec3 RandomUnitVec();
  static vec3 RandomInHemisphere(const vec3 &normal);
  static vec3 RandomInUnitDisc();

  static vec3 Zero() { return Vector3Zero(); }
};

// Implemented in vec3.cpp
vec3 operator*(const vec3 &, const float);
vec3 operator*(const float, const vec3 &);

// Inline definitions
inline vec3 vec3::Normalize() const { return Vector3Normalize(*this); }

inline float vec3::SqrLen() const {
  auto [x, y, z] = *this;
  return x * x + y * y + z * z;
}

inline float vec3::Len() const { return sqrt(SqrLen()); }

inline vec3 vec3::operator-() const { return Vector3Negate(*this); }

inline vec3 vec3::operator-(const vec3 &rVec3) const {
  return Vector3Subtract(*this, rVec3);
}

inline vec3 vec3::operator+(const vec3 &rVec3) const {
  return Vector3Add(*this, rVec3);
}

inline vec3 vec3::operator/(const float f) const {
  auto [x, y, z] = *this;
  return vec3(x / f, y / f, z / f);
}

inline vec3 vec3::operator*(const vec3 &rVec3) const {
  auto [lX, lY, lZ] = *this;
  auto [rX, rY, rZ] = rVec3;
  return vec3(lX * rX, lY * rY, lZ * rZ);
}

inline vec3 &vec3::operator+=(const vec3 &rVec3) {
  auto [rX, rY, rZ] = rVec3;
  x += rX;
  y += rY;
  z += rZ;
  return *this;
}

inline vec3 &vec3::operator/=(const float f) {
  x /= f;
  y /= f;
  z /= f;
  return *this;
}

inline vec3 &vec3::operator*=(const float f) {
  x *= f;
  y *= f;
  z *= f;
  return *this;
}

inline void from_json(const json &jsonVec, vec3 &v) {
  v.x = jsonVec["x"].get<float>();
  v.y = jsonVec["y"].get<float>();
  v.z = jsonVec["z"].get<float>();
}

inline void to_json(json &j, const vec3 &v) {
  j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
}