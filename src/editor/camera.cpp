#include "camera.h"
#include "Constants.h"
#include "Scene.h"
#include "Util.h"

#include <glm/gtc/matrix_transform.hpp>  // for perspective
#include <glm/gtx/fast_trigonometry.hpp> // for wrapAngle
#include <imgui.h>

using namespace rt::constants;

void rt::editor::Camera::DrawFrameOutline() const {
  if (!frameSizeAffectsCrop)
    return;

  auto constexpr thickness = 5.0;
  // Account for titlebar
  auto constexpr titlebarClearence = 4;
  auto rectX                       = (editorWidth - _imageWidth) / 2;
  auto rectY                       = (editorHeight - _imageHeight) / 2;
  DrawRectangleLinesEx(::Rectangle(rectX, rectY + (thickness * titlebarClearence), _imageWidth - thickness,
                                   _imageHeight - (thickness * titlebarClearence)),
                       thickness, MAGENTA);
}

void rt::editor::Camera::RenderImgui() {
  if (ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
    if (ImGui::DragFloat3("lookFrom", &rtCamera.lookFrom.x, 0.05)) {
      rtCamera.lookAt = rtCamera.lookFrom + rtCamera.localForward;
    }

    ImGui::Combo("Camera type", (int *)&controlType, controlTypeLabels,
                 static_cast<int>(rt::editor::Camera::ControlType::controlTypesCount), 0);

    if (controlType == ControlType::lookAtPoint) {
      ImGui::DragFloat3("rtCamera.lookAt", &rtCamera.lookAt.x, 0.05);
    } else {
      if (ImGui::DragFloat2("rotation", &angle.x, 0.05f))
        lookAtAngle(angle);
    }

    ImGui::DragFloat("Vertical FOV", &rtCamera.vFov, 1.0, 5.0, 179.0f);

    {
      ImGui::Checkbox("Frame size affects crop?", &frameSizeAffectsCrop);
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip(
            "Whether setting a smaller/bigger frame size crops/expands the rendered image or just renders what you "
            "see at a lower/higher resolution.");
      }
    }

    ImGui::Separator();

    ImGui::DragFloat("Camera speed multiplier", &movementSpeedMultiplier, 0.1, 1, 100);

    ImGui::DragFloat("Camera boost multiplier", &boostMultiplier, 0.1, 1, 10);

    ImGui::DragInt2("frame size", &_imageWidth);
  }

  ImGui::End();
}

rt::editor::Camera::Camera(int editorWidth, int editorHeight, int imageWidth, int imageHeight, Scene const &initialScene)
    : editorWidth(editorWidth), editorHeight(editorHeight), _imageWidth(imageWidth), _imageHeight(imageHeight),
      rtCamera(initialScene.cam), controlType(ControlType::flyCam) {
  updateFromRtCamera(rtCamera);
}

void rt::editor::Camera::updateFromRtCamera(rt::Camera const &sceneCamera) {
  rtCamera = sceneCamera;
  rtCamera.RecomputeLocalBases();

  forwardToAngle(localForward());
  lookAtAngle(angle);
}

void rt::editor::Camera::UpdateRtCamera() {
  rtCamera.aspectRatio = static_cast<float>(_imageWidth) / _imageHeight;
  rtCamera.RecomputeLocalBases();
}

void rt::editor::Camera::Fwd(float deltaTime) {
  rtCamera.lookFrom += moveDir * deltaTime;
  UpdateRtCamera();
}

void rt::editor::Camera::Bck(float deltaTime) {
  rtCamera.lookFrom += -moveDir * deltaTime;
  UpdateRtCamera();
}

void rt::editor::Camera::lookAtAngle(vec3 angle) {
  // x = r * cos(90 - angle.y) * sin(90 - angle.x)
  // y = r * cos(90 - angle.x)
  // z = r * sin(90 - angle.y) * sin(90 - angle.x)
  auto const dir = vec3{
      cos(pi / 2 - angle.y),
      cos(angle.x - pi / 2),
      sin(pi / 2 - angle.y),
  };

  rtCamera.lookAt = rtCamera.lookFrom + dir;
  UpdateRtCamera();
}

void rt::editor::Camera::forwardToAngle(vec3 fwd) {
  assert(fwd.SqrLen() > 0);

  angle.x = pi / 2 + std::acos(fwd.y);
  angle.y = std::atan2(fwd.x, fwd.z);

  if (angle.x >= pi) {
    angle.x = pi - angle.x;
  }
  angle.y = glm::wrapAngle(angle.y);
}

void rt::editor::Camera::MouseLook(Vector2 mousePositionDelta) {
  angle.x += (mousePositionDelta.y * -rotSensitity.y);
  angle.y += (mousePositionDelta.x * -rotSensitity.x);

  angle.x = std::clamp(angle.x, xAngleClampMin * DEG2RAD, xAngleClampMax * DEG2RAD);
  angle.y = glm::wrapAngle(angle.y);

  lookAtAngle(angle);
}

void rt::editor::Camera::Update(float dt) {
  float speedMultiplier = movementSpeedMultiplier;

  if (IsKeyDown(KEY_LEFT_SHIFT))
    speedMultiplier *= boostMultiplier;

  auto [upChange, fwdChange, rgtChange] = rtCamera.getScaledDirectionVectors(dt * speedMultiplier);

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    HideCursor();
    preHideCursorLocation = GetMousePosition();
  }

  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
    if (IsKeyDown(KEY_SPACE)) {
      rtCamera.lookFrom += upChange;
      rtCamera.lookAt += upChange;
    }
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
      rtCamera.lookFrom -= upChange;
      rtCamera.lookAt -= upChange;
    }
    if (IsKeyDown(KEY_W)) {
      rtCamera.lookFrom += fwdChange;
      rtCamera.lookAt += fwdChange;
    }
    if (IsKeyDown(KEY_S)) {
      rtCamera.lookFrom -= fwdChange;
      rtCamera.lookAt -= fwdChange;
    }
    if (IsKeyDown(KEY_A)) {
      rtCamera.lookFrom -= rgtChange;
      rtCamera.lookAt -= rgtChange;
    }
    if (IsKeyDown(KEY_D)) {
      rtCamera.lookFrom += rgtChange;
      rtCamera.lookAt += rgtChange;
    }

    MouseLook(GetMouseDelta());
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
    SetMousePosition(preHideCursorLocation.x, preHideCursorLocation.y);
    ShowCursor();
  }
}

Camera3D rt::editor::Camera::toRaylibCamera3D() const {
  return Camera3D{.position   = rtCamera.lookFrom,
                  .target     = rtCamera.lookAt,
                  .up         = {0, 1, 0},
                  .fovy       = rtCamera.vFov,
                  .projection = CAMERA_PERSPECTIVE};
}

glm::mat4 rt::editor::Camera::getProjectionMatrix() const {
  return glm::perspective(glm::radians(rtCamera.vFov), static_cast<float>(editorWidth) / editorHeight, 0.01f, 100.0f);
}

float rt::editor::Camera::GetCorrectedCropFov() const {
  // Clamp the scale in case the image is larger than the editor window.
  auto const scale = std::min(static_cast<float>(_imageHeight) / editorHeight, 1.0f);
  return glm::degrees(2 * std::atan(scale * std::tan(DegressToRadians(rtCamera.vFov) / 2)));
}

rt::Camera rt::editor::Camera::toSceneCamera() const {
  auto const aspectRatio = static_cast<float>(_imageWidth) / _imageHeight;
  auto       vFov        = rtCamera.vFov;

  if (frameSizeAffectsCrop) {
    vFov = GetCorrectedCropFov();
  }

  return rt::Camera(rtCamera.lookFrom, rtCamera.lookAt, rtCamera.worldUp, vFov, aspectRatio, rtCamera.aperature,
                    rtCamera.focusDist, rtCamera.time0, rtCamera.time1);
}
