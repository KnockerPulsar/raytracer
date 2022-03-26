#include "vec3.h"
#include "../Defs.h"
#include "../Util.h"

vec3::vec3() {}

vec3::vec3(const float x, const float y, const float z) : Vector3{x, y, z} {}

vec3::vec3(const float f) : vec3(f, f, f) {}

vec3::vec3(Vector3 conv) : Vector3{conv} {}

bool vec3::NearZero() const {
  const float smol = 1e-8;
  return fabs(x) < smol && fabs(y) < smol && fabs(z) < smol;
}

vec3 vec3::Reflect(const vec3 &norm) const {
  float b_scale = vec3::DotProd(norm, *this);
  vec3  b       = norm * b_scale;
  return *this - b * 2;
}

vec3 vec3::Refract(const vec3 &n, float etai_over_etat) {
  float cos_theta      = fmin(DotProd(-*this, n), 1.0);
  vec3  r_out_perp     = etai_over_etat * (*this + cos_theta * n);
  vec3  r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.SqrLen())) * n;
  return r_out_perp + r_out_parallel;
}

float vec3::DotProd(const vec3 &left, const vec3 &right) {
  return Vector3DotProduct(left, right);
}

vec3 vec3::CrsProd(const vec3 &left, const vec3 &right) {
  return Vector3CrossProduct(left, right);
}

vec3 vec3::Random() {
  return vec3(RandomFloat(), RandomFloat(), RandomFloat());
}

vec3 vec3::Random(const float min, const float max) {
  return vec3(
      RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max));
}

vec3 vec3::RandomInUnitSphere() {
  while (true) {
    vec3 p = vec3::Random(-1, 1);
    // No implementation for vec3.squared_lem :(
    if (p.SqrLen() >= 1)
      continue;
    return p;
  }
}

vec3 vec3::RandomUnitVec() { return RandomInUnitSphere().Normalize(); }

vec3 vec3::RandomInHemisphere(const vec3 &normal) {
  vec3 in_unit_sphere = RandomInUnitSphere();
  if (DotProd(normal, in_unit_sphere) > 0.0)
    return in_unit_sphere;
  else
    return -in_unit_sphere;
}

vec3 vec3::RandomInUnitDisc() {
  while (true) {
    vec3 p(RandomFloat(-1, 1), RandomFloat(-1, 1), 0);
    if (p.SqrLen() >= 1)
      continue;
    return p;
  }
}

// Binary operators
vec3 operator*(const vec3 &lVec3, const float rFloat) {
  auto [x, y, z] = lVec3;
  return vec3(x * rFloat, y * rFloat, z * rFloat);
}

vec3 operator*(const float lFloat, const vec3 &rVec3) {
  return operator*(rVec3, lFloat);
}
