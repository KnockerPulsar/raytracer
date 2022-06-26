#pragma once
#include "raylib.h"
#include "raymath.h"

namespace Editor {
  struct CameraEx {
    Camera _camera;

    Vector3 fwd;
    Vector3 rgt;
    float   movScale     = 1 / 10.0f;
    Vector3 angle        = {0, 0, 0};
    Vector2 rotSensitity = {0.003f, 0.003f};

    Vector3 &pos = _camera.position;
    Vector3 &tgt = _camera.target;
    Vector3 &up  = _camera.up;
    Vector3  localUp;

    const float yAngleClampMin = 89.0f;
    const float yAngleClampMax = -89.0f;
    const float panningDivider = 51.0f;
  };
} // namespace Editor