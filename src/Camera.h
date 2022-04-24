#pragma once
#include "Defs.h"
#include "IRasterizable.h"
#include "Ray.h"
#include "data_structures/vec3.h"
#include <raylib.h>
#include <vector>

namespace rt {
  class Camera {
  public:
    vec3 lookFrom, lookAt, moveDir;
    vec3 lower_left_corner, horizontal, vertical;
    vec3 u, v, w, vUp;

    float time0, time1;
    float vFov, // vertical field-of-view in degrees
        aspectRatio, aperature, focusDist, lensRadius;

    Camera() = default;
    Camera(vec3  lookFrom,
           vec3  lookAt,
           vec3  vUp,
           vec3  moveDir,
           float vFov,
           float aspectRatio,
           float aperature,
           float focusDist,
           float time0 = 0.0,
           float time1 = 1.0)
        : lookFrom(lookFrom), lookAt(lookAt), vUp(vUp), moveDir(moveDir), vFov(vFov), aspectRatio(aspectRatio),
          aperature(aperature), focusDist(focusDist), time0(time0), time1(time1) {

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

    Camera(nlohmann::json cameraJson, float aspectRatio)
        : Camera(cameraJson["look_from"].get<vec3>(),
                 cameraJson["look_at"].get<vec3>(),
                 cameraJson["v_up"].get<vec3>(),
                 cameraJson["move_dir"].get<vec3>(),
                 cameraJson["fov"].get<float>(),
                 aspectRatio,
                 cameraJson["aperature"].get<float>(),
                 cameraJson["focus_dist"].get<float>(),
                 cameraJson["time0"].get<float>(),
                 cameraJson["time1"].get<float>()) {}

    void Update() {
      float theta = DegressToRadians(vFov);

      // https://raytracing.github.io/images/fig-1.14-cam-view-geom.jpg
      float h               = tan(theta / 2);
      float viewport_height = 2.0 * h;
      float viewport_width  = aspectRatio * viewport_height;

      // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
      w = (lookFrom - lookAt).Normalize();
      u = vec3::CrsProd(vUp, w).Normalize();
      v = vec3::CrsProd(w, u);

      horizontal        = focusDist * viewport_width * u;
      vertical          = focusDist * viewport_height * v;
      lower_left_corner = this->lookFrom - horizontal / 2 - vertical / 2 - focusDist * w;
    }

    void Fwd(float deltaTime) {
      lookFrom += moveDir * deltaTime;
      Update();
    }
    void Bck(float deltaTime) {
      lookFrom += -moveDir * deltaTime;
      Update();
    }

    rt::Ray GetRay(float s, float t) const;

    void Rasterize(std::vector<sPtr<Hittable>> rasterizables);
  };

  inline void to_json(json &j, const Camera &c) {
    j = json{
        "camera",
        {
            {"look_from", c.lookFrom},
            {"look_at", c.lookAt},
            {"v_up", c.vUp},
            {"focus_dist", c.focusDist},
            {"aperature", c.aperature},
            {"move_dir", c.moveDir},
            {"fov", c.vFov},
            {"time0", c.time0},
            {"time1", c.time1},
        },
    };
  }
} // namespace rt
