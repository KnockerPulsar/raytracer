#include "Utils.h"
#include "CameraEx.h"
#include "Cuboid.h"
#include "raymath.h"
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <optional>
#include <raylib.h>

namespace Editor {
  std::string Vector3ToStr(Vector3 vec) {
    return "{ " + std::to_string(vec.x) + "," + std::to_string(vec.y) + "," +
           std::to_string(vec.z) + " }";
  }

  std::string Vector2ToStr(Vector2 vec) {
    return "{ " + std::to_string(vec.x) + "," + std::to_string(vec.y) + " }";
  }

  Vector3 GetMousePosWorld(CameraEx &cam, int screenWidth, int screenHeight,
                           int planePos, int axisPerpToPlane) {

    Vector3 mousePosDir =
        GetMouseRay(GetMousePosition(), cam._camera).direction;

    float denom = (axisPerpToPlane == 0)   ? mousePosDir.x
                  : (axisPerpToPlane == 1) ? mousePosDir.y
                  : (axisPerpToPlane == 2) ? mousePosDir.z
                                           : 1e-4;
    float pos   = (axisPerpToPlane == 0)   ? cam.pos.x
                  : (axisPerpToPlane == 1) ? cam.pos.y
                  : (axisPerpToPlane == 2) ? cam.pos.z
                                           : 1e-4;
    float t     = std::fabs(pos - planePos) / fabs(denom);

    Vector3 worldSpace = Vector3Add(cam.pos, Vector3Scale(mousePosDir, t));
    return worldSpace;
  }

  // 0: X, 1: Y, 2: Z
  std::tuple<Vector2, Vector2, Color> GetHUDAxis(CameraEx &cam,
                                                 Vector2   screenDims,
                                                 Vector2   offsetPcnts,
                                                 float axisLength, int axis) {
    Vector3 axis3DOrigin = Vector3Add(cam.pos, Vector3Scale(cam.fwd, 1.0f));
    Vector2 axis2DOrigin = GetWorldToScreen(axis3DOrigin, cam._camera);

    const float xOffsetPcnt = offsetPcnts.x;
    const float yOffsetPcnt = offsetPcnts.y;
    const float halfWidth   = screenDims.x / 2.0f;
    const float halfHeight  = screenDims.y / 2.0f;

    const float xOffset = halfWidth * xOffsetPcnt;
    const float yOffset = -halfHeight * yOffsetPcnt;

    Vector2 axis2DEnd;
    Color   axisColor;
    if (axis == 0) {
      axis2DEnd = GetWorldToScreen(Vector3Add(axis3DOrigin, {axisLength, 0, 0}),
                                   cam._camera);
      axisColor = RED;
    } else if (axis == 1) {
      axis2DEnd = GetWorldToScreen(Vector3Add(axis3DOrigin, {0, axisLength, 0}),
                                   cam._camera);
      axisColor = GREEN;
    } else if (axis == 2) {
      axis2DEnd = GetWorldToScreen(Vector3Add(axis3DOrigin, {0, 0, axisLength}),
                                   cam._camera);
      axisColor = BLUE;
    }

    axis2DOrigin = Vector2Add(axis2DOrigin, {xOffset, yOffset});
    axis2DEnd    = Vector2Add(axis2DEnd, {xOffset, yOffset});

    return std::make_tuple(axis2DOrigin, axis2DEnd, axisColor);
  }

  std::tuple<Vector3, Vector3, Color>
  GetAxisAtPoint(Vector3 pos, Vector2 screenDims, float axisLength,
                 float axisThickness, int axis) {
    Vector3 axis3DOrigin = pos;
    Vector3 axis3DSize; // Cuboid extents in X, Y, and Z directions;

    Color axisColor;
    if (axis == 0) {
      axis3DSize = {axisLength, axisThickness, axisThickness};
      // axis3DOrigin = Vector3Add(axis3DOrigin, {axisLength / 2.0f, 0, 0});
      axisColor = RED;
    } else if (axis == 1) {
      axis3DSize = {axisThickness, axisLength, axisThickness};
      // axis3DOrigin = Vector3Add(axis3DOrigin, {0, axisLength / 2.0f, 0});
      axisColor = GREEN;
    } else if (axis == 2) {
      axis3DSize = {axisThickness, axisThickness, axisLength};
      // axis3DOrigin = Vector3Add(axis3DOrigin, {0, 0, axisLength / 2.0f});
      axisColor = BLUE;
    }

    return std::make_tuple(axis3DOrigin, axis3DSize, axisColor);
  }

  BoundingBox GetBoundingBox(Vector3 pos, Vector3 size) {
    return (BoundingBox){
        (Vector3){pos.x - size.x / 2, pos.y - size.y / 2, pos.z - size.z / 2},
        (Vector3){pos.x + size.x / 2, pos.y + size.y / 2, pos.z + size.z / 2}};
  }

  CameraEx InitCamera() {
    // Define the camera to look into our 3d world (position, target, up vector)
    Vector3 cameraFwd = Vector3Normalize((Vector3){3, -100, 4});
    Camera  camera    = {
            .position = {3.0f, 5.0f, 5.0f},
            .target   = {cameraFwd},
            .up       = (Vector3){0.0f, 1.0f, 0.0f},
            .fovy     = 60.0f,
    };

    Vector3 cameraRgt =
        Vector3Normalize(Vector3CrossProduct(cameraFwd, camera.up));

    Editor::CameraEx cam = {
        ._camera  = camera,
        .fwd      = cameraFwd,
        .rgt      = cameraRgt,
        .movScale = 10,
    };

    return cam;
  }

  vector<Cuboid> GenerateTestWorld(int maxCols) {
    // Generates some random columns
    vector<Cuboid> cuboids(maxCols);

    for (int i = 0; i < maxCols; i++) {
      cuboids[i].height = (float)GetRandomValue(1, 12);
      cuboids[i].position =
          (Vector3){(float)GetRandomValue(-15, 15), cuboids[i].height / 2.0f,
                    (float)GetRandomValue(-15, 15)};
      cuboids[i].color =
          (Color){static_cast<unsigned char>(GetRandomValue(20, 255)),
                  static_cast<unsigned char>(GetRandomValue(10, 55)), 30, 255};
    }

    return cuboids;
  }

  void DrawWallsAndCubes(const vector<Cuboid> &cuboids) {

    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){32.0f, 32.0f},
              LIGHTGRAY); // Draw ground
    DrawCube((Vector3){-16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f,
             BLUE); // Draw a blue wall
    DrawCube((Vector3){16.0f, 2.5f, 0.0f}, 1.0f, 5.0f, 32.0f,
             LIME); // Draw a green wall
    DrawCube((Vector3){0.0f, 2.5f, 16.0f}, 32.0f, 5.0f, 1.0f,
             GOLD); // Draw a yellow wall
    for (int i = 0; i < cuboids.size(); i++) {
      // Basic cube drawing
      DrawCube(cuboids[i].position, 2.0f, cuboids[i].height, 2.0f,
               cuboids[i].color);
    }
  }

  std::optional<int> CheckObjectClicked(const vector<Cuboid> &cuboids,
                                        bool pickCube, Ray r) {
    std::optional<int> hitCubeID    = {};
    bool               hitSomething = false;
    float              minHitDist   = std::numeric_limits<float>::infinity();
    for (int i = 0; i < cuboids.size(); i++) {
      // If we just pressed the left mouse button, check for collisions with all
      // boxes (doesn't check handles)
      if (pickCube) {
        RayCollision coll = GetRayCollisionBox(
            r, Editor::GetBoundingBox(cuboids[i].position,
                                      {2.2f, cuboids[i].height, 2.2f}));

        // If we hit, store the ID of the object we hit and raise a flag.
        if (coll.hit && coll.distance < minHitDist) {

          minHitDist   = coll.distance;
          hitCubeID    = i;
          hitSomething = true;
        }
      }

      // If we hit, draw something to show the object is selected.
      if (hitCubeID.has_value()) {
        const Cuboid &hitCuboid = cuboids[hitCubeID.value()];
        DrawCube(hitCuboid.position, 2.2f, hitCuboid.height, 2.2f,
                 Fade(GREEN, 0.05f));
        // DrawCubeWires(positions[i], 2.2f, heights[i], 2.2f, GREEN);}
      }
    }
    return hitCubeID;
  }

  void DrawPickedObjectAxes(const Camera &camera, const Cuboid &pickedCube,
                            float screenWidth, float screenHeight) {
    // Position the handles at the hit object,
    // then check for collision with every handle

    for (int ax = 0; ax < 3; ++ax) {
      auto [objAxStart, objAxExtents, objAxCol] = Editor::GetAxisAtPoint(
          pickedCube.position, {screenWidth, screenHeight}, 4.0f, 0.5f, ax);

      // Vector2 axisStartScreenSpace = GetWorldToScreen(objAxStart, camera);
      // Vector2 axisEndScreenSpace =
      //     GetWorldToScreen(Vector3Add(objAxExtents, objAxStart), camera);

      // DrawLineEx(axisStartScreenSpace, axisEndScreenSpace, 5.0f, objAxCol);
      DrawCubeV(objAxStart, objAxExtents, objAxCol);
      DrawCubeWires(objAxStart, objAxExtents.x * 1.1f, objAxExtents.y * 1.1f,
                    objAxExtents.z * 1.1f, MAGENTA);
    }
  }

  void CheckPickedObjectDrag(const Cuboid       &pickedCube,
                             std::optional<int> &draggingAxis, const Ray &r,
                             std::optional<RayCollision> &draggingColl,
                             bool &dragging, float screenWidth,
                             float screenHeight) {
    for (int ax = 0; ax < 3; ++ax) {
      auto [objAxStart, objAxExtents, objAxCol] = Editor::GetAxisAtPoint(
          pickedCube.position, {screenWidth, screenHeight}, 4.0f, 0.5f, ax);

      if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        draggingColl = GetRayCollisionBox(
            r, Editor::GetBoundingBox(objAxStart, objAxExtents));

        if (draggingColl.value().hit && !dragging) {
          draggingAxis = ax;
          dragging     = true;
        }
      }
    }
  }

  
} // namespace Editor