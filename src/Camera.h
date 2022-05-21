#pragma once
#include "Constants.h"
#include "Defs.h"
#include "Hittable.h"
#include "HittableList.h"
#include "IRasterizable.h"
#include "Ray.h"
#include "data_structures/vec3.h"
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <vector>

namespace rt {
  enum CameraControlType { flyCam, lookAt, controlTypesCount };

  class Camera {
  public:
    static inline Hittable *selectedObject; // TODO: Move this out

    vec3 lookFrom, lookAt, moveDir;
    vec3 lower_left_corner, horizontal, vertical;
    vec3 u, v, w, vUp;
    vec3 rgt;

    float time0, time1;
    float vFov, // vertical field-of-view in degrees
        aspectRatio, aperature, focusDist, lensRadius;

    constexpr static const float xAngleClampMin = -89.0f;
    constexpr static const float xAngleClampMax = 89.0f;
    float                        panningDivider = 51.0f;
    float                        movScale       = 10.0f;
    vec3                         angle          = {0, 0, 0}; // Used to rotate the camera using the mouse
    Vector2                      rotSensitity   = {0.003f, 0.003f};

    CameraControlType   controlType         = CameraControlType::flyCam;
    inline static const char *controlTypeLabels[] = {"flyCam", "lookAt"};

    inline static vec3 lineStart, lineEnd;

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
      float h              = tan(theta / 2);
      float viewportHeight = 2.0 * h;
      float viewportWidth  = aspectRatio * viewportHeight;

      // https://raytracing.github.io/images/fig-1.16-cam-view-up.jpg
      w = (lookFrom - lookAt).Normalize();
      u = vec3::CrsProd(vUp, w).Normalize();
      v = vec3::CrsProd(w, u);

      horizontal        = focusDist * viewportWidth * u;
      vertical          = focusDist * viewportHeight * v;
      lower_left_corner = this->lookFrom - horizontal / 2 - vertical / 2 - focusDist * w;
      rgt               = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(lookAt, lookFrom), vUp));
    }

    void Fwd(float deltaTime) {
      lookFrom += moveDir * deltaTime;
      Update();
    }

    void Bck(float deltaTime) {
      lookFrom += -moveDir * deltaTime;
      Update();
    }

    Camera3D toCamera3D() const {
      return Camera3D{
          .position = lookFrom, .target = lookAt, .up = vUp, .fovy = vFov, .projection = CAMERA_PERSPECTIVE};
    }

    Hittable *CastRay(Vector2 mousePos, Vector2 screenDims, HittableList *world) const {
      // auto [s, t] = mouseST;

      // Ray r = GetRay(s, t);

      float s =  mousePos.x /  (screenDims.x-1);
      float t =  (screenDims.y - mousePos.y) /  (screenDims.y-1);

      ::Ray raylibRay = GetMouseRay(mousePos, toCamera3D());
      Ray   r = {raylibRay.position, raylibRay.direction, 0};
      rt::Camera::lineStart = r.origin.toGlm();
      rt::Camera::lineEnd   = r.At(1000).toGlm();


      rt::HitRecord rec;
      world->Hit(r, -INFINITY, INFINITY, rec);

      return rec.closestHit;
    }

    rt::Ray GetRay(float s, float t) const;

    void Rasterize(std::vector<sPtr<Hittable>> rasterizables);

    void                         UpdateEditorCamera(float dt);
    void                         RaylibRotateCamera(Vector2 mousePositionDelta);
    void                         RenderImgui(HittableList *objectList);
    std::tuple<vec3, vec3, vec3> getScaledDirectionVectors(float dt) const;
    glm::mat4                    getCameraTransform() const;
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
