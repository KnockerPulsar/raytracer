#pragma once

#include "../../vendor/nlohmann-json/json.hpp"
#include "../Defs.h"
#include "../Util.h"
#include <raylib.h>
#include <raymath.h>
#include <sys/types.h>
#include "../../vendor/glm/glm/glm.hpp" 

/**
 * @brief Represents any 3 component object (X,Y,Z) or (R,G,B)
 */
class vec3 : public Vector3 {
public:
  vec3();                                      // Creates a 3 component vector with all elements zeroed
  vec3(float);                                 // Creates a 3 componenet vector with all elements set to the given float
  vec3(float, float, float);                   // Sets the each component to the corresponding parameter
  vec3(const Vector3 &);                       // Copy constructor for raylib's Vector3
  vec3(const glm::vec3&);

  static vec3 Zero() { return Vector3Zero(); } // Returns a vector with all components set to zero
  glm::vec3 toGlm() const ;
  Color toRaylibColor(u_char alpha ) const;

  bool NearZero() const;                // Checks if all components of the vector are less than epsilon (Constants.h)
  vec3 Reflect(const vec3 &norm) const; // Reflects the vector about a given axisI

  // Refracts the vector about the given normal with the given coefficient
  vec3 Refract(const vec3 &n, float etaIOverEtaT) const;

  inline vec3  Normalize() const; // Normalizes the vector so that its length is 1
  inline float SqrLen() const;    // Returns x^2 + y^2 + z^2
  inline float Len() const;       // Returns the length of the vector `sqrt(SqrLen())`

  inline vec3 operator-() const;              // Negates all components
  inline vec3 operator-(const vec3 &v) const; // Element-wise subtraction
  inline vec3 operator+(const vec3 &v) const; // Element-wise addition
  inline vec3 operator*(const vec3 &v) const; // Element-wise multiplication
  inline vec3 operator/(float f) const;       // Divides all components by the given float, returns a new vector

  inline vec3 &operator+=(const vec3 &v); // Element-wise increment
  inline vec3 &operator-=(const vec3 &v); // Element-wise decrement
  inline vec3 &operator/=(float f);       // Divides all components by the given float, mutates the calling vector
  inline vec3 &operator*=(float f);       // Multiplies all components by the given float, mutates the calling vector

  static float DotProd(const vec3 &, const vec3 &); // Dot product on the two given vectors
  static vec3  CrsProd(const vec3 &, const vec3 &); // Cross product on the two given vectors

  static vec3 Random();                               // Random vector
  static vec3 Random(float min, float max);           // Random vector where each component is in the given range
  static vec3 RandomInUnitSphere();                   // Random vector with length <= 1
  static vec3 RandomUnitVec();                        // Random vector with length == 1
  static vec3 RandomInHemisphere(const vec3 &normal); // Random vector in the same hemisphere as the given normal
  static vec3 RandomInUnitDisc();                     // Random vector in unit sphere with the z component set to zero
};

// Implemented in vec3.cpp due to linker errors
vec3 operator*(const vec3 &, float); // Multiplies all components by the given float, returns a new vector
vec3 operator*(float, const vec3 &); // Same but with a different order

// Inline definitions
// ==================

inline vec3 vec3::Normalize() const { return Vector3Normalize(*this); }

inline float vec3::SqrLen() const {
  auto [x, y, z] = *this;
  return x * x + y * y + z * z;
}

inline float vec3::Len() const { return sqrt(SqrLen()); }

inline vec3 vec3::operator-() const { return Vector3Negate(*this); }

inline vec3 vec3::operator-(const vec3 &v) const { return Vector3Subtract(*this, v); }

inline vec3 vec3::operator+(const vec3 &v) const { return Vector3Add(*this, v); }

inline vec3 vec3::operator*(const vec3 &v) const {
  auto [lX, lY, lZ] = *this;
  auto [rX, rY, rZ] = v;
  return vec3(lX * rX, lY * rY, lZ * rZ);
}

inline vec3 vec3::operator/(const float f) const {
  auto [x, y, z] = *this;
  return vec3(x / f, y / f, z / f);
}

inline vec3 &vec3::operator+=(const vec3 &v) {
  auto [rX, rY, rZ] = v;
  x += rX;
  y += rY;
  z += rZ;
  return *this;
}

inline vec3 &vec3::operator-=(const vec3 &v) {
  *this += -v;
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

// Json serialization / deserialzation

inline void from_json(const json &jsonVec, vec3 &v) {
  v.x = jsonVec["x"].get<float>();
  v.y = jsonVec["y"].get<float>();
  v.z = jsonVec["z"].get<float>();
}

inline void to_json(json &j, const vec3 &v) { j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}}; }