#pragma once
#include "Ray.h"
#include "Vec3.h"

namespace raytracer {
  class Camera {
  public:
    Vec3  lookFrom, lookAt, moveDir;
    float time0, time1;

    Camera() = default;
    Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 vUp, Vec3 moveDir, float vFov,
           float aspectRatio, float aperature, float focusDist,
           float time0 = 0.0, float time1 = 1.0)
        : lookFrom(lookFrom), lookAt(lookAt), vUp(vUp), moveDir(moveDir),
          vFov(vFov), aspectRatio(aspectRatio), aperature(aperature),
          focusDist(focusDist), time0(time0), time1(time1) {

      float theta = DegressToRadians(vFov);

      // https://raytracing.github.io/images/fig-1.14-cam-view-geom.jpg
      float h               = tan(theta / 2);
      float viewport_height = 2.0 * h;
      float viewport_width  = aspectRatio * viewport_height;

      // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
      Update();

      lensRadius    = aperature / 2;
      this->lookAt  = lookAt;
      this->moveDir = moveDir;
    }

    void Update() {
      float theta = DegressToRadians(vFov);

      // https://raytracing.github.io/images/fig-1.14-cam-view-geom.jpg
      float h               = tan(theta / 2);
      float viewport_height = 2.0 * h;
      float viewport_width  = aspectRatio * viewport_height;

      // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
      w = (lookFrom - lookAt).Normalize();
      u = Vec3::CrsProd(vUp, w).Normalize();
      v = Vec3::CrsProd(w, u);

      horizontal = focusDist * viewport_width * u;
      vertical   = focusDist * viewport_height * v;
      lower_left_corner =
          this->lookFrom - horizontal / 2 - vertical / 2 - focusDist * w;
    }

    void Fwd(float deltaTime) {
      lookFrom += moveDir * deltaTime;
      Update();
    }
    void Bck(float deltaTime) {
      lookFrom += -moveDir * deltaTime;
      Update();
    }

    raytracer::Ray GetRay(float s, float t) const;

  private:
    Vec3 lower_left_corner, horizontal, vertical;
    Vec3 u, v, w, vUp;

    float vFov, // vertical field-of-view in degrees
        aspectRatio, aperature, focusDist, lensRadius;
  };
} // namespace raytracer
