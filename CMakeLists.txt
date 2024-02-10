cmake_minimum_required(VERSION 3.0.0)   # Selects the minimum version of CMake required to run this file
project(Raytracer VERSION 0.1.0)          # Here we select the project name and version

# Here we select C++17 with all the standards required and all compiler-specific extensions disabled
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -funroll-loops -Wall -flto -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall")

CMAKE_POLICY(SET CMP0074 NEW)

# A variable with all our source files that are common between executable targets (examples)
set(SOURCES 
)

# Define the directories in which to search for the included headers
include_directories(
  src/
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY          ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(${PROJECT_NAME} src/main.cc ${SOURCES})

target_link_libraries(
  ${PROJECT_NAME} 
)
