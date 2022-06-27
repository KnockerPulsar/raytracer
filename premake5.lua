workspace "rt"
configurations {"Debug", "Release"}
require("modules/premake-ecc/ecc")

project "Raytracer"
  kind "WindowedApp"
  language "C++"
  cppdialect "c++17"

  -- All these are system libraries
  -- Raylib is also installed as a system library but statically to provide portability.
  if os.host() == "linux" then
    links {"pthread", "GL", "m", "rt", "X11", "dl", "raylib:static"}
  end
  

  if os.host() == "windows" then
    links { "pthread" ,"opengl32" ,"gdi32" ,"winmm" ,"mwindows" ,"libgcc:static" ,"stdc++", "raylib:static"}
  end

  files {
    "src/**.h",
    "src/**.cpp",
    
    "vendor/rlImGui/imgui/*.cpp",
    "vendor/rlImGui/imgui/*.h",

    "vendor/rlImGui/imgui/backends/imgui_impl_opengl3_loader.h",
    "vendor/rlImGui/imgui/backends/imgui_impl_opengl3.cpp",
    "vendor/rlImGui/imgui/backends/imgui_impl_opengl3.h",
  
    "vendor/rlImGui/*.cpp",
    "vendor/rlImGui/*.h",

    "vendor/imguizmo/*",

    "vendor/argumentum/src/*"
  }

  includedirs {
    "src/",
    "src/**"
  }

  filter {"configurations:Debug"}
    defines {"DEBUG", "FAST_EXIT"}
    symbols "On"

  filter {"configurations:Release"}
    defines {"NDEBUG", "FAST_EXIT"}
    floatingpoint "Fast"
    flags {"LinkTimeOptimization"}
    symbols "Off"
    optimize "Speed"
