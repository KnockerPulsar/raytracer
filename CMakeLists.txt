cmake_minimum_required(VERSION 3.5.0)   # Selects the minimum version of CMake required to run this file
project(Raytracer VERSION 0.1.0)          # Here we select the project name and version

# Here we select C++23 with all the standards required and all compiler-specific extensions disabled
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(FAST_EXIT)
add_compile_definitions(GAMMA_CORRECTION)


set(CMAKE_CXX_FLAGS_RELEASE "-flto=auto -ffast-math -O3 -Ofast -ffloat-store -march=native -frename-registers -funroll-loops -fopenmp")

# set(CMAKE_BUILD_TYPE Debug)
CMAKE_POLICY(SET CMP0074 NEW)

# Profiling stuff
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

add_subdirectory(vendor/glm/glm)

# A variable with all our source files that are common between executable targets (examples)
set(SOURCES
  src/Scene.cpp
  src/app.cpp
  src/AABB.cpp
  src/Camera.cpp
  src/HittableList.cpp
  src/Ray.cpp
  src/AsyncRenderData.cpp
  src/GroupPanel.cpp
  src/Transformation.cpp
  src/BVHNode.cpp

  src/data_structures/vec3.cpp

  src/objects/Box.cpp
  src/objects/Sphere.cpp
  src/objects/AARect.cpp

  src/editor/editor.cpp
  src/raytracer.cpp
  src/editor/Utils.cpp
  src/editor/PreviewTexture.cpp
  src/editor/camera.cpp

  vendor/argumentum/src/argparser.cpp

  vendor/ImGuiFileDialog/ImGuiFileDialog.cpp

  vendor/rlImGui/rlImGui.cpp

  vendor/imgui/imgui.cpp
  vendor/imgui/imgui_widgets.cpp
  vendor/imgui/imgui_draw.cpp
  vendor/imgui/imgui_tables.cpp
  vendor/imgui/backends/imgui_impl_opengl3.cpp

  vendor/ImGuizmo/ImGuizmo.cpp
)

# Define the directories in which to search for the included headers
include_directories(
  src/

  vendor/
  vendor/rlImGui/
  vendor/ImGuizmo/
  vendor/imgui/
  vendor/ImGuiFileDialog
  vendor/argumentum/include/
  vendor/stb_image/
  vendor/glm
)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
  SYMBOLIC
)

# For each example, we add an executable target
# Each target compiles one example source file and the common & vendor source files
# Then we link GLFW with each target
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

target_link_libraries(
  ${PROJECT_NAME}
  -lraylib
  -lpthread
  -lGL
  -lm
  -lrt
  -lX11
  -ldl
  glm
)
