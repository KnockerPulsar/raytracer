# raytracer
Experimenting with creating a CPU raytracer. Main resources:
- [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html)
- [_Ray Tracing: The Next Week_](https://raytracing.github.io/books/RayTracingTheNextWeek.html)


# Showcase

![Cornell](./cornell.png)
*Cornell box. This added support for quads, boxes, lights, textures, and more! Rendered with 10k samples and 50 bounces*

![Weekend final](./weekend_final.png)
*Showcase of an earlier version of the renderer, implements all of "Raytracing in one weekend"'s core functions + multi-threading*

![Texture mapping](./two_spheres.png)
*Texture mapping for spheres!*

![Screenshot with glass, metallic, and diffuse balls](./screenshot.png)
*Earlier version with glass, metallic, and diffuse balls. Notice how the glass ball is hollow?*

# Features
- Currently renders only spheres (more to come).
- Three material types: diffuse, metallic, dielectric. 
- Supports spatial acceleration with BVHs.
- Texture mapping.

# Dependencies
- [raylib](https://github.com/raysan5/raylib) for drawing and Vector3 class library
