cmake_minimum_required(VERSION 3.0.0)   # Selects the minimum version of CMake required to run this file
project(Raytracer VERSION 0.1.0)          # Here we select the project name and version

# Here we select C++17 with all the standards required and all compiler-specific extensions disabled
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(FAST_EXIT)
add_compile_definitions(GAMMA_CORRECTION)


set(CMAKE_CXX_FLAGS_RELEASE "-O3 -funroll-loops -Wall -flto -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall")

CMAKE_POLICY(SET CMP0074 NEW)

# Profiling stuff
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
# add_subdirectory(vendor/glm)

# find_package(raylib REQUIRED) # Requires at least version 3.0

# A variable with all our source files that are common between executable targets (examples)
set(SOURCES 
)

# Define the directories in which to search for the included headers
include_directories(
  src/
  external/
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY          ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG    ${PROJECT_SOURCE_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  ${PROJECT_SOURCE_DIR}/release)
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
