#pragma once
#include "Util.h"
#include "Vec3.h"
#include <cmath>
#include <type_traits>
namespace raytracer {
  class Perlin {
  public:
    Perlin() {

      ranVec = new Vec3[pointCount];

      for (int i = 0; i < pointCount; ++i) {
        ranVec[i] = Vec3::Random(-1, 1).Normalize();
      }

      permX = PerlinGeneratePerm();
      permY = PerlinGeneratePerm();
      permZ = PerlinGeneratePerm();
    }

    ~Perlin() {
      delete[] ranVec;
      delete[] permX;
      delete[] permY;
      delete[] permZ;
    }

    float Noise(const Vec3 &p) const {

      // Gets the fractional value of each coordinate
      float u = p.x - floor(p.x);
      float v = p.y - floor(p.y);
      float w = p.z - floor(p.z);

      // Gets the integer value of each coordinate
      int i = floor(p.x);
      int j = floor(p.y);
      int k = floor(p.z);

      Vec3 c[2][2][2];

      for (int di = 0; di < 2; di++) {
        for (int dj = 0; dj < 2; dj++) {
          for (int dk = 0; dk < 2; dk++) {
            // Hashing???

            c[di][dj][dk] =
                ranVec[permX[(i + di) & 255] ^ permY[(j + dj) & 255] ^
                       permZ[(k + dk) & 255]];
          }
        }
      }
      return PerlinInterp(c, u, v, w);
    }

    float Turb(const Vec3 &p, int depth = 7) const {
      float accum  = 0.0f;
      Vec3  tempP  = p;
      float weight = 1.0f;

      for (int i = 0; i < depth; i++) {
        accum += weight * Noise(tempP);
        weight *= 0.5;
        tempP *= 2;
      }

      return fabs(accum);
    }

  private:
    static const int pointCount = 256;
    Vec3            *ranVec;
    int             *permX;
    int             *permY;
    int             *permZ;

    static int *PerlinGeneratePerm() {
      auto *p = new int[pointCount];
      for (int i = 0; i < Perlin::pointCount; i++)
        p[i] = i;

      Permute(p, pointCount);
      return p;
    }

    static void Permute(int *p, int n) {
      for (int i = n - 1; i > 0; i--) {
        int target = RandomInt(0, i);
        std::swap(p[target], p[i]);
      }
    }

    // u,v,w are the fractional part of a Vec3's x,y,z coordinates
    // c is a 3D matrix of (random?) values
    static float PerlinInterp(Vec3 c[2][2][2], float u, float v, float w) {

      float uu    = u * u * (3 - 2 * u);
      float vv    = v * v * (3 - 2 * v);
      float ww    = w * w * (3 - 2 * w);
      float accum = 0.0f;

      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
          for (int k = 0; k < 2; k++) {
            Vec3 weightV(u - i, v - j, w - k);

            // clang-format off
            // This looks like interpolation...
            accum +=  (i * u + (1 - i) * (1 - u)) 
                    * (j * v + (1 - j) * (1 - v)) 
                    * (k * w + (1 - k) * (1 - w)) 
                    * Vec3::DotProd(c[i][j][k], weightV);

            // clang-format on
          }
      return accum;
    }
  };
} // namespace raytracer