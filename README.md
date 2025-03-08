# raytracer
Experimenting with creating a CPU raytracer. Main resources:
- [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [_Ray Tracing: The Next Week_](https://raytracing.github.io/books/RayTracingTheNextWeek.html)


# Showcase

![Cornell](./screenshots/cornell.png)
*Cornell box. This version added support for quads, boxes, lights, textures, and more! Rendered with 10k samples and 50 bounces*

![Weekend final](./screenshots/weekend_final.png)
*Showcase of an earlier version of the renderer, implements all of "Raytracing in one weekend"'s core functions + multi-threading*

![Texture mapping](./screenshots/two_spheres.png)
*Texture mapping for spheres!*

![Screenshot with glass, metallic, and diffuse balls](./screenshots/screenshot.png)  
*Earlier version with glass, metallic, and diffuse balls. Notice how the glass ball is hollow?*

# Features
- Currently renders only spheres (more to come).
- Three material types: diffuse, metallic, dielectric. 
- Supports spatial acceleration with BVHs.
- Texture mapping.

# Dependencies
- [raylib](https://github.com/raysan5/raylib) (4.0) for drawing and Vector3 class library
- [glm](https://github.com/g-truc/glm) (latest) for transforms
- [rlImGui](https://github.com/raylib-extras/rlImGui) (imgui docking branch @77637fd9363ac7ef7b6fb6419b760602192a38bd) for imgui support
- [nlohmann-json](https://github.com/nlohmann/json) for json support
- [argumentum](https://github.com/mmahnic/argumentum) (v0.3.1) For CLI options
- [imguizmo](https://github.com/CedricGuillemet/ImGuizmo) (vendored) for translation/rotation/scale gizmos
- [stb_image](https://github.com/nothings/stb/tree/master) (vendored) for image loading and storing
- [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog) (v0.64), pretty self explanatory
