cmake_minimum_required(VERSION 3.5.0)   # Selects the minimum version of CMake required to run this file
project(Raytracer VERSION 0.1.0)          # Here we select the project name and version

# Here we select C++17 with all the standards required and all compiler-specific extensions disabled
set(CMAKE_CXX_STANDARD 17)
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
add_subdirectory(vendor/glm)

# A variable with all our source files that are common between executable targets (examples)
set(SOURCES 
  src/Scene.cpp
  src/app.cpp
  src/AABB.cpp
  src/Camera.cpp
  src/HittableList.cpp
  src/RenderAsync.cpp
  src/Ray.cpp
  src/AsyncRenderData.cpp
  src/GroupPanel.cpp

  src/data_structures/vec3.cpp

  src/objects/Box.cpp
  src/objects/Sphere.cpp
  src/objects/AARect.cpp
  
  src/editor/editor.cpp
  src/editor/Utils.cpp

  vendor/argumentum/src/argparser.cpp

  vendor/ImGuiFileDialog/ImGuiFileDialog.cpp

  vendor/rlImGui/rlImGui.cpp

  vendor/rlImGui/imgui/imgui.cpp
  vendor/rlImGui/imgui/imgui_widgets.cpp
  vendor/rlImGui/imgui/imgui_draw.cpp
  vendor/rlImGui/imgui/imgui_tables.cpp
  vendor/rlImGui/imgui/backends/imgui_impl_opengl3.cpp

  vendor/imguizmo/ImGuizmo.cpp
)

# Define the directories in which to search for the included headers
include_directories(
  src/

  vendor/rlImGui/
  vendor/imguizmo/
  vendor/rlImGui/imgui/  
  vendor/nlohmann-json/
  vendor/ImGuiFileDialog
  vendor/argumentum/include/
  vendor/ImGuiFileDialog/stb/
  vendor/stb_image
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY          ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG    ${PROJECT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${PROJECT_SOURCE_DIR}/bin/release)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

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
)
