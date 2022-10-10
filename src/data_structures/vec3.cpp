#include "vec3.h"
#include "../Defs.h"
#include "../Util.h"
#include <cstdint>
#include <sys/types.h>

vec3::vec3() {}

vec3::vec3(const float x, const float y, const float z) : Vector3{x, y, z} {}

vec3::vec3(const float f) : vec3(f, f, f) {}

vec3::vec3(const Vector3 &conv) : Vector3{conv} {}

vec3::vec3(const glm::vec3 &glmVec) : vec3(glmVec.x, glmVec.y, glmVec.z) {}

vec3::vec3(const Color &color) : vec3(float(color.r) / 255, float(color.g) / 255, float(color.b) / 255) {}

bool vec3::NearZero() const { return fabs(x) < epsilon && fabs(y) < epsilon && fabs(z) < epsilon; }

glm::vec3 vec3::toGlm() const { return glm::vec3(x, y, z); }

glm::vec4 vec3::toPoint() const { return glm::vec4(toGlm(), 1); }
glm::vec4 vec3::toVec() const { return glm::vec4(toGlm(), 0); }

Color vec3::toRaylibColor(u_char alpha) const {
  vec3 temp = *this * 255;
  temp.x    = std::min(temp.x, 255.0f);
  temp.y    = std::min(temp.y, 255.0f);
  temp.z    = std::min(temp.z, 255.0f);
  return Color{(u_char)temp.x, (u_char)temp.y, (u_char)temp.z, alpha};
}

vec3 vec3::Reflect(const vec3 &norm) const {
  float bScale = vec3::DotProd(norm, *this);
  vec3  b      = norm * bScale;
  return *this - b * 2;
}

vec3 vec3::Refract(const vec3 &n, float etaIOverEtaT) const {
  float cosTheta     = fmin(DotProd(-*this, n), 1.0);
  vec3  rOutPerp     = etaIOverEtaT * (*this + cosTheta * n);
  vec3  rOutParallel = -sqrt(fabs(1.0 - rOutPerp.SqrLen())) * n;
  return rOutPerp + rOutParallel;
}

float vec3::DotProd(const vec3 &left, const vec3 &right) { return Vector3DotProduct(left, right); }

vec3 vec3::CrsProd(const vec3 &left, const vec3 &right) { return Vector3CrossProduct(left, right); }

vec3 vec3::projectOntoPlane(const vec3 &planeNormal) const {
  return *this - vec3::DotProd(*this, planeNormal) * planeNormal;
}

vec3 vec3::Random() { return vec3(RandomFloat(), RandomFloat(), RandomFloat()); }

vec3 vec3::Random(const float min, const float max) {
  return vec3(RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max));
}

vec3 vec3::RandomInUnitSphere() {
  // If we normalize it when its length > 1, the results could be skewed towards length 1 vectors...
  while (true) {
    vec3 p = vec3::Random(-1, 1);
    if (p.SqrLen() >= 1)
      continue;
    return p;
  }
}

vec3 vec3::RandomUnitVec() { return RandomInUnitSphere().Normalize(); }

vec3 vec3::RandomInHemisphere(const vec3 &normal) {
  vec3 inUnitSphere = RandomInUnitSphere();
  if (DotProd(normal, inUnitSphere) > 0.0)
    return inUnitSphere;
  return -inUnitSphere;
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

vec3 operator*(const float lFloat, const vec3 &rVec3) { return operator*(rVec3, lFloat); }
