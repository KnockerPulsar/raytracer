#include "AABB.h"
#include "Ray.h"

#define USE_SSE 1

  template<unsigned i>
float vecElem( __m128 V)
{
  // shuffle V so that the element that you want is moved to the least-
  // significant element of the vector (V[0])
  V = _mm_shuffle_ps(V, V, _MM_SHUFFLE(i, i, i, i));
  // return the value in V[0]
  return _mm_cvtss_f32(V);
}

#define VMUL(a,b) _mm_mul_ps(a,b)
#define VSUB(a,b) _mm_sub_ps(a,b)
#define VAND(a,b) _mm_and_ps(a,b)
#define VMAX(a,b) _mm_max_ps(a,b)
#define VMIN(a,b) _mm_min_ps(a,b)

namespace rt {
  AABB::AABB(const vec3 &a, const vec3 &b) {

    b3.min = a;
    b3.max = b;

    Pad();
  }

  AABB::AABB(std::vector<vec3> points) {
    b3.min = vec3(infinity, infinity, infinity);

    b3.max = -b3.min;
    for (auto &&point : points) {
      b3.min.x = fmin(point.x, b3.min.x);
      b3.min.y = fmin(point.y, b3.min.y);
      b3.min.z = fmin(point.z, b3.min.z);

      b3.max.x = fmax(point.x, b3.max.x);
      b3.max.y = fmax(point.y, b3.max.y);
      b3.max.z = fmax(point.z, b3.max.z);
    }

    Pad();
  }

  bool AABB::Hit(const Ray &r, float tMin, float tMax) const {

#if USE_SSE
    static __m128 mask4 = _mm_cmpeq_ps(_mm_setzero_ps(), _mm_set_ps(1, 0, 0, 0));

    __m128 t1 = VMUL(VSUB(VAND(b4.min, mask4), r.origin.O4), r.rDirection.rD4);
    __m128 t2 = VMUL(VSUB(VAND(b4.max, mask4), r.origin.O4), r.rDirection.rD4);

    __m128 vmax4 = VMAX(t1, t2);
    __m128 vmin4 = VMIN(t1, t2);

    float  tmax = std::min(vecElem<0>(vmax4), std::min(vecElem<1>(vmax4), vecElem<2>(vmax4)));
    float  tmin = std::max(vecElem<0>(vmin4), std::max(vecElem<1>(vmin4), vecElem<2>(vmin4)));

    return (tmax >= tmin) && tmin <= r.time && tmax > 0;
#else
    {
      auto invD = 1.0f / r.direction.D3.x;
      auto t0   = (b3.min.x - r.origin.O3.x) * invD;
      auto t1   = (b3.max.x - r.origin.O3.x) * invD;
      if (invD < 0.0f)
        std::swap(t0, t1);
      tMin = t0 > tMin ? t0 : tMin;
      tMax = t1 < tMax ? t1 : tMax;
      if (tMax <= tMin)
        return false;
    }
    {
      auto invD = 1.0f / r.direction.D3.y;
      auto t0   = (b3.min.y - r.origin.O3.y) * invD;
      auto t1   = (b3.max.y - r.origin.O3.y) * invD;
      if (invD < 0.0f)
        std::swap(t0, t1);
      tMin = t0 > tMin ? t0 : tMin;
      tMax = t1 < tMax ? t1 : tMax;
      if (tMax <= tMin)
        return false;
    }
    {
      auto invD = 1.0f / r.direction.D3.z;
      auto t0   = (b3.min.z - r.origin.O3.z) * invD;
      auto t1   = (b3.max.z - r.origin.O3.z) * invD;
      if (invD < 0.0f)
        std::swap(t0, t1);
      tMin = t0 > tMin ? t0 : tMin;
      tMax = t1 < tMax ? t1 : tMax;
      if (tMax <= tMin)
        return false;
    }
    return true;
#endif
  }

  AABB AABB::SurroundingBox(AABB a, AABB b) {
    vec3 smol(fmin(a.b3.min.x, b.b3.min.x), fmin(a.b3.min.y, b.b3.min.y), fmin(a.b3.min.z, b.b3.min.z));

    vec3 big(fmax(a.b3.max.x, b.b3.max.x), fmax(a.b3.max.y, b.b3.max.y), fmax(a.b3.max.z, b.b3.max.z));

    return AABB(smol, big);
  }

  void AABB::Pad() {
    // In case the points are coplanar
    float eps = 1e-6f;
    if (abs(b3.min.x - b3.max.x) < eps) {
      b3.min.x -= eps;
      b3.max.x += eps;
    }
    if (abs(b3.min.y - b3.max.y) < eps) {
      b3.min.y -= eps;
      b3.max.y += eps;
    }
    if (abs(b3.min.z - b3.max.z) < eps) {
      b3.min.z -= eps;
      b3.max.z += eps;
    }
  }
} // namespace rt