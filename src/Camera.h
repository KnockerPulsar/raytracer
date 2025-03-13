#pragma once
#include "Defs.h"
#include "Ray.h"
#include "data_structures/vec3.h"

#include <raylib.h>
#include <raymath.h>

#include <cmath>

namespace rt {

  class Camera {
  private:
    float lensRadius;

  public:
    vec3 lookFrom, lookAt;
    vec3 lowerLeftCorner, horizontal, vertical;
    vec3 localRight, localUp, localForward, worldUp;

    float time0, time1;
    float vFov; // vertical field-of-view in degrees
    float aspectRatio, aperature, focusDist;

    Camera() = default;

    Camera(
        vec3  lookFrom,
        vec3  lookAt,
        vec3  vUp,
        float vFov,
        float aspectRatio,
        float aperature,
        float focusDist,
        float time0 = 0.0,
        float time1 = 1.0
    );

    Camera(nlohmann::json cameraJson, float aspectRatio);

    rt::Ray GetRay(float s, float t) const;

    void                         RenderImgui();
    std::tuple<vec3, vec3, vec3> getScaledDirectionVectors(float dt) const;
  };

  inline void to_json(json &j, const Camera &c) {
    j = json{
        "camera",
        {{"look_from", c.lookFrom},
         {"look_at", c.lookAt},
         {"v_up", c.worldUp},
         {"focus_dist", c.focusDist},
         {"aperature", c.aperature},
         {"fov", c.vFov},
         {"time0", c.time0},
         {"time1", c.time1}},
    };
  }
} // namespace rt
