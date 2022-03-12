#include "Vec3.h"
#include "Util.h"

Vec3::Vec3() {}

Vec3::Vec3(const float x, const float y, const float z) : Vector3{x, y, z} {}

Vec3::Vec3(const float f) : Vec3(f, f, f) {}

Vec3::Vec3(Vector3 conv) : Vector3{conv} {}

bool Vec3::NearZero() const {
  const float smol = 1e-8;
  return fabs(x) < smol && fabs(y) < smol && fabs(z) < smol;
}

Vec3 Vec3::Reflect(const Vec3 &norm) const {
  float b_scale = Vec3::DotProd(norm, *this);
  Vec3  b       = norm * b_scale;
  return *this - b * 2;
}

Vec3 Vec3::Refract(const Vec3 &n, float etai_over_etat) {
  float cos_theta      = fmin(DotProd(-*this, n), 1.0);
  Vec3  r_out_perp     = etai_over_etat * (*this + cos_theta * n);
  Vec3  r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.SqrLen())) * n;
  return r_out_perp + r_out_parallel;
}

float Vec3::DotProd(const Vec3 &left, const Vec3 &right) {
  return Vector3DotProduct(left, right);
}

Vec3 Vec3::CrsProd(const Vec3 &left, const Vec3 &right) {
  return Vector3CrossProduct(left, right);
}

Vec3 Vec3::Random() {
  return Vec3(RandomFloat(), RandomFloat(), RandomFloat());
}

Vec3 Vec3::Random(const float min, const float max) {
  return Vec3(
      RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max));
}

Vec3 Vec3::RandomInUnitSphere() {
  while (true) {
    Vec3 p = Vec3::Random(-1, 1);
    // No implementation for Vec3.squared_lem :(
    if (p.SqrLen() >= 1)
      continue;
    return p;
  }
}

Vec3 Vec3::RandomUnitVec() { return RandomInUnitSphere().Normalize(); }

Vec3 Vec3::RandomInHemisphere(const Vec3 &normal) {
  Vec3 in_unit_sphere = RandomInUnitSphere();
  if (DotProd(normal, in_unit_sphere) > 0.0)
    return in_unit_sphere;
  else
    return -in_unit_sphere;
}

Vec3 Vec3::RandomInUnitDisc() {
  while (true) {
    Vec3 p(RandomFloat(-1, 1), RandomFloat(-1, 1), 0);
    if (p.SqrLen() >= 1)
      continue;
    return p;
  }
}

Vec3 Vec3::FromJson(nlohmann::json jsonVec) {
  return Vec3(jsonVec["x"].get<float>(),
              jsonVec["y"].get<float>(),
              jsonVec["z"].get<float>());
}

// Binary operators

Vec3 operator*(const Vec3 &lVec3, const float rFloat) {
  auto [x, y, z] = lVec3;
  return Vec3(x * rFloat, y * rFloat, z * rFloat);
}

Vec3 operator*(const float lFloat, const Vec3 &rVec3) {
  return operator*(rVec3, lFloat);
}
