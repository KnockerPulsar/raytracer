#include "editor.h"
#include "../../vendor/glm/glm/glm.hpp"
#include "../../vendor/glm/glm/gtc/type_ptr.hpp"
#include "../../vendor/glm/glm/gtx/transform.hpp"
#include "../../vendor/imguizmo/ImGuizmo.h"
#include "../../vendor/rlImGui/rlImGui.h"
#include "../BVHNode.h"
#include "../Camera.h"
#include "../Hittable.h"
#include "../HittableList.h"
#include "../materials/DiffuseLight.h"
#include "../objects/Box.h"
#include "../objects/Plane.h"
#include "../objects/Sphere.h"
#include "Utils.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <raylib.h>
#include <rlgl.h>
#include <vector>

namespace rt {

  void Editor::Rasterize() {

    BeginTextureMode(screenRT);
    ClearBackground(BLACK);

    BeginMode3D(cam->toRaylibCamera3D());
    {
      DrawGrid(10, 10);

      if (!currentScene->skysphereTexture.empty()) {
        currentScene->skysphere->transformation.translate = cam->lookFrom;
        currentScene->drawSkysphere();
      }

      auto rasterizables = currentScene->worldRoot->getChildrenAsList();
      for (auto &&raster : rasterizables) {
        raster->RasterizeTransformed(raster->transformation);
      }

      auto AABBs = currentScene->worldRoot->getChildrenAABBs();

      AABB rootAABB;
      currentScene->worldRoot->BoundingBoxTransformed(0, 1, rootAABB);
      AABBs.push_back(rootAABB);

      for (auto &&bb : AABBs) {
        DrawBoundingBox({bb.min, bb.max}, {255, 0, 255, 255});
      }

      DrawLine3D(rt::Camera::lineStart, rt::Camera::lineEnd, BLUE);
    }
    EndMode3D();
    EndTextureMode();
  }

  void Editor::RenderImgui() {

    // ImGui::Begin(
    //     "Editor", 0, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);

    // float imguiWidth = ImGui::GetContentRegionAvail().x;
    // float height     = imguiWidth * float(screenRT.texture.height) / screenRT.texture.width;

    // ImGui::End();

    // TODO: Use correct parameters when viewport is figured out
    // Currently parameters are not used inside the function.
    UpdateViewportRect(-1, -1);

    RenderViewport();

    ImGui::Begin("Objects");
    {
      if (dynamic_cast<BVHNode *>(currentScene->worldRoot) != nullptr) {
        if (ImGui::Button("Regenerate BVH", {-1, 0})) {

          // Regenerate tree
          currentScene->worldRoot = currentScene->worldRoot->addChild(nullptr);
        }
      }

      ImGui::Combo(("##" + EditorUtils::GetIDFromPointer(this)).c_str(),
                   (int *)&selectedAddableObject,
                   addableObjectTypes,
                   AddableObjectsTypesCount,
                   -1);
      ImGui::SameLine();
      if (ImGui::Button("+", {-1, 0})) {
        addObject();
      }

      auto objects = currentScene->worldRoot->getChildrenAsList();
      for (auto &&o : objects) {
        std::string idPlusName = o->name + "##" + EditorUtils::GetIDFromPointer(&o);

        // Size of {-1,0} to use full width
        if (ImGui::Button(idPlusName.c_str())) {
          selectedObject = o.get();
        }

        ImGui::SameLine();

        // Remove current object from world and rebuild
        if (ImGui::Button(("x##" + EditorUtils::GetIDFromPointer(&o)).c_str())) {
          currentScene->worldRoot = currentScene->worldRoot->removeChild(o);
        }

        ImGui::Separator();
      }
    }
    ImGui::End();

    RaytraceSettingsImgui();

    SelectedObjectGizmo();
  }

  void Editor::SelectedObjectGizmo() const {
    if (selectedObject != nullptr) {

      const auto selectedObjectUniqueName = selectedObject->name + "##" + EditorUtils::GetIDFromPointer(selectedObject);

      glm::mat4 model = selectedObject->transformation.getModelMatrix();
      ImGuizmo::BeginFrame();

      ImGui::Begin("Selected object", 0, ImGuiWindowFlags_AlwaysAutoResize);

      // Tell ImGuizmo to draw in the foreground
      // ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

      // Set viewport for ImGuizmo for proper Gizmo placement
      ImGuizmo::SetRect(viewportMin.x, viewportMin.y, viewportSize.x, viewportSize.y);

      // Tell ImGui to not draw outside the viewport
      ImGui::PushClipRect(viewportMin, viewportMax, false);

      // clang-format off
        ImGuizmo::Manipulate(
          glm::value_ptr(cam->getViewMatrix()),
    glm::value_ptr(cam->getProjectionMatrix()),
     ImGuizmo::TRANSLATE,
          ImGuizmo::LOCAL,
        glm::value_ptr(model)
        );
      // clang-format on
      if (ImGuizmo::IsUsing()) {

        vec3 translation;
        vec3 rotation;
        vec3 scale;

        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), &translation.x, &rotation.x, &scale.x);

        selectedObject->transformation.translate = translation;
        selectedObject->transformation.rotate    = rotation;
      }

      ImGui::Button(selectedObjectUniqueName.c_str(), {-1, 0});
      selectedObject->OnImgui();

      ImGui::End();
    }
  }

  void Editor::CheckInput() {
    ImGuiIO &io = ImGui::GetIO();
    // std::cout << io.WantCaptureMouse << std::endl;
    if (IsMouseButtonPressed(0) && !io.WantCaptureMouse
        /*&& mouseInsideEditorViewport*/) {
      // Trial-and-error'd my way through figuring out these

      float mouseX = GetMouseX() - viewportMin.x;
      float mouseY = GetMouseY() - viewportMin.y;

      float mouseU = mouseX / (viewportMax.x - viewportMin.x);
      float mouseV = mouseY / (viewportMax.y - viewportMin.y);

      mouseX = mouseU * (GetScreenWidth());
      mouseY = mouseV * (GetScreenHeight());

      // std::cout << mouseU << "," << mouseV << "\n";

      selectedObject = CastRay({mouseX, mouseY});
    }
  }

  Hittable *Editor::CastRay(Vector2 mousePos) const {

    ::Ray raylibRay = GetMouseRay(mousePos, cam->toRaylibCamera3D());
    Ray   r         = {raylibRay.position, raylibRay.direction, 0};

    rt::Camera::lineStart = r.origin.toGlm();
    rt::Camera::lineEnd   = r.At(1000).toGlm();

    rt::HitRecord rec;
    currentScene->worldRoot->Hit(r, -INFINITY, INFINITY, rec);

    return rec.closestHit;
  }

  void Editor::onUpdate() {

    CheckInput();

    if (cam->controlType == rt::Camera::ControlType::flyCam)
      cam->Update(GetFrameTime());

    Rasterize();

    BeginDrawing();
    rlImGuiBegin();

    RenderImgui();
    cam->RenderImgui();

    rlImGuiEnd();
    EndDrawing();
  }

  void Editor::addObject() {
    Hittable      *added;
    sPtr<Material> mat             = std::make_shared<DiffuseLight>(vec3(10, 0, 10));
    std::string    hittableAddress = EditorUtils::GetIDFromPointer(added);

    switch (selectedAddableObject) {
    case Box: {
      added       = new rt::Box(vec3(-0.5), vec3(0.5), mat);
      added->name = "Added Box @ " + hittableAddress;
      break;
    }

    case Sphere: {
      added       = new rt::Sphere(1, vec3(0), mat);
      added->name = "Added Sphere @ " + hittableAddress;
      break;
    }

    case Plane: {
      added       = new rt::Plane(vec3(0), 1, 1, mat);
      added->name = "Added Plane @ " + hittableAddress;
      break;
    }

    case AddableObjectsTypesCount:
    default: {
    }
    }

    auto *newRoot = currentScene->worldRoot->addChild(sPtr<Hittable>(added));
    if (newRoot == nullptr) {
      std::cerr << "Attempting to add a child to a world root that's not a BVHNode or a HittableList\n";
    } else {
      // currentScene->oldWorldRoot = currentScene->worldRoot;
      currentScene->worldRoot = newRoot;
    }
  }

  void Editor::UpdateViewportRect(float imguiWidth, float height) {

    viewportMin = {0, 0};
    viewportMax = ImVec2(GetScreenWidth(), GetScreenHeight());

    // TODO: Uncomment this for proper viewport raycasting when you figure viewports out.
    // viewportMin = {0, 0};
    // viewportMax = {imguiWidth, height};
    // viewportMin.x += ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x;
    // viewportMin.y += ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y;
    // viewportMax.x += ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x;
    // viewportMax.y += ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y;

    viewportSize = {viewportMax.x - viewportMin.x, viewportMax.y - viewportMin.y};

    mouseInsideEditorViewport = ImGui::IsMouseHoveringRect(viewportMin, viewportMax);
  }

  void Editor::RenderViewport() {
    // TODO: Figure out a way to render to an ImGui image AND register gizmo hits
    // ImGui::Image(&screenRT.texture.id, {float(viewportSize.x), float(viewportSize.y)}, {0, 1}, {1, 0});
    // ImGui::GetForegroundDrawList()->AddRect(viewportMin, viewportMax, IM_COL32(255, 255, 0, 255));

    ClearBackground(BLACK);
    // Flip on Y axis since ImGui and opengl/raylib use different axis
    DrawTextureRec(
        screenRT.texture, {0, 0, float(screenRT.texture.width), -float(screenRT.texture.height)}, {0, 0}, WHITE);
  }

  void Editor::RaytraceSettingsImgui() { currentScene->settings.OnImgui(); }
} // namespace rt