#pragma once

#include "common/IState.h"
#include "app.h"
#include "camera.h"
#include "common/Defs.h"
#include "editor/IImguiDrawable.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <random>
#include <raylib.h>

#include <glm/glm.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include <optional>

class Vector2;


namespace rt {

  namespace raytracer {
    class Scene;
  }

  namespace editor {
    class Scene;
    class Material;
    class Hittable;


    class Editor : public IState {
    public:
      struct ViewState {
        struct FileMenu {
          bool shouldOpen{false};
        };

        struct ViewMenu {
          bool shouldOpen{false};
          bool raytracingSettings{true}, cameraSettings{true}, objectList{true};
        };

        struct BuiltInMenu {
          bool shouldOpen{false};
        };

        FileMenu    fileMenu;
        ViewMenu    viewMenu;
        BuiltInMenu builtInMenu;
      };

      Editor(App *const app, CliConfig const &config);
      ~Editor();

      /*
        Checks for inputs, updates the editor camera and its GUI,
        rasterizes object, renders editor GUI.
      */
      virtual void onUpdate() override;
      virtual void onExit() override;

      static std::optional<sPtr<editor::Material>> MaterialChanger();

      auto compileScene() const -> std::unique_ptr<raytracer::Scene>;

    private:
      struct Config : public rt::IImguiDrawable {
        // All defaults are defined in `CliConfig`
        glm::ivec2   const editorSize;

        uint64_t     numThreads;
        bool         potatoMode;
        bool         saveOnRender;

        // If not set, defaults from the scene will be used
        uint64_t numSamples;
        uint64_t maxDepth;

        uint64_t seed;

        Config(CliConfig const &cliConfig, Scene const &scene)
            : editorSize(cliConfig.editorSize), numThreads(cliConfig.numThreads), saveOnRender(cliConfig.saveOnRender),
              numSamples(cliConfig.numSamples), maxDepth(cliConfig.maxDepth),
              seed(cliConfig.seed.value_or(std::random_device{}())) {}

        void OnImgui() override {
          ImGui::Begin("Other settings");
          ImGui::Checkbox("Save on render?", &saveOnRender);
          ImGui::DragInt("Number of threads", reinterpret_cast<int*>(&numThreads), 1, 1, 128);
          ImGui::DragInt("Samples per pixel", reinterpret_cast<int *>(&numSamples), 1, 1, 500);
          ImGui::DragInt("Maximum depth", reinterpret_cast<int *>(&maxDepth), 1, 1, 100);
          ImGui::InputScalar("Seed", ImGuiDataType_U64, &seed);
          ImGui::End();
        }
      };

      enum AddableObjectsTypes : uint8_t { Box, Sphere, Plane, AddableObjectsTypesCount };
      inline static const char *addableObjectTypes[] = {
          "Box",
          "Sphere",
          "Plane",
      };

      // Calls overloaded `Rasterize()` function for each object
      void Rasterize();
      void SelectedObjectGizmo();
      // Checks for editor related inputs (mouse clicks for picking, gizmo modes)
      void CheckInput();
      // Renders editor specific GUI
      void RenderImgui();
      void FileMenuImGui();
      void FileMenuCheckInputs();
      void ViewMenuCheckInputs();
      void ViewMenuImGui();
      void BuiltInMenuImGui();
      void BuiltInMenuCheckInputs();
      void SelectedObjectImGui();
      // Tests a ray against the world and returns whatever it hits (can be null)
      editor::Object *CastRay(Vector2 mousePos);
      void AddObjectImgui();
      void RenderViewport() const;
      void ObjectListImgui();
      void TopMenuImgui();

      editor::Object *selectedObject = nullptr;

      AddableObjectsTypes selectedAddableObject = Box;

      ImGuizmo::MODE      imguizmoMode = ImGuizmo::MODE::LOCAL;
      ImGuizmo::OPERATION imguizmoOp   = ImGuizmo::OPERATION::TRANSLATE;

      inline static const Color colors[] = {
          GOLD,   GRAY,      DARKPURPLE, SKYBLUE,  MAROON, ORANGE, GREEN, RED,       DARKGRAY, PURPLE, LIGHTGRAY, BEIGE,
          VIOLET, DARKBROWN, MAGENTA,    RAYWHITE, LIME,   WHITE,  BROWN, DARKGREEN, DARKBLUE, BLUE,   YELLOW,    PINK,
      };

      static const int numColors = sizeof(colors) / sizeof(colors[0]);

      std::unique_ptr<Scene> scene;
      editor::Camera  camera;
      RenderTexture2D rasterRT;

      ViewState viewState;
      Config    config;

    }; // namespace Editor
  } // namespace editor
} // namespace rt
