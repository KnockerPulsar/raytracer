#pragma once

#include <raymath.h>

class Vec3 : public Vector3 {
 public:
  Vec3();
  Vec3(const float);
  Vec3(const float, const float, const float);
  Vec3(Vector3);

  bool NearZero() const;
  Vec3 Reflect(const Vec3& norm) const;
  Vec3 Refract(const Vec3& n, float etai_over_etat);

  inline Vec3 Normalize() const;
  inline float SqrLen() const;
  inline float Len() const;

  inline Vec3 operator-() const;
  inline Vec3 operator-(const Vec3&) const;
  inline Vec3 operator+(const Vec3&) const;
  inline Vec3 operator/(const float) const;
  inline Vec3 operator*(const Vec3&) const;

  inline Vec3& operator+=(const Vec3&);
  inline Vec3& operator/=(const float);
  inline Vec3& operator*=(const float);

  static float DotProd(const Vec3&, const Vec3&);
  static inline Vec3 Random();
  static inline Vec3 Random(const float min, const float max);
  static Vec3 RandomInUnitSphere();
  static Vec3 RandomUnitVec();
  static Vec3 RandomInHemisphere(const Vec3& normal);

  static Vec3 Zero() { return Vector3Zero(); }
};

// Implemented in Vec3.cpp
Vec3 operator*(const Vec3&, const float);
Vec3 operator*(const float, const Vec3&);

// Inline definitions
inline Vec3 Vec3::Normalize() const {
  return Vector3Normalize(*this);
}

inline float Vec3::SqrLen() const {
  auto [x, y, z] = *this;
  return x * x + y * y + z * z;
}

inline float Vec3::Len() const {
  return sqrt(SqrLen());
}

inline Vec3 Vec3::operator-() const {
  return Vector3Negate(*this);
}

inline Vec3 Vec3::operator-(const Vec3& rVec3) const {
  return Vector3Subtract(*this, rVec3);
}

inline Vec3 Vec3::operator+(const Vec3& rVec3) const {
  return Vector3Add(*this, rVec3);
}

inline Vec3 Vec3::operator/(const float f) const {
  auto [x, y, z] = *this;
  return Vec3(x / f, y / f, z / f);
}

inline Vec3 Vec3::operator*(const Vec3& rVec3) const {
  auto [lX, lY, lZ] = *this;
  auto [rX, rY, rZ] = rVec3;
  return Vec3(lX * rX, lY * rY, lZ * rZ);
}

inline Vec3& Vec3::operator+=(const Vec3& rVec3) {
  auto [rX, rY, rZ] = rVec3;
  x += rX;
  y += rY;
  z += rZ;
  return *this;
}

inline Vec3& Vec3::operator/=(const float f) {
  x /= f;
  y /= f;
  z /= f;
  return *this;
}

inline Vec3& Vec3::operator*=(const float f) {
  x *= f;
  y *= f;
  z *= f;
  return *this;
}
