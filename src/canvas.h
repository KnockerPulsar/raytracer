#include <raylib.h>
#include "../include/raylib-cpp/include/raylib-cpp.hpp"

namespace raytracer
{
    class Canvas
    {
    private:
        raylib::Vector3 position;
        std::vector<std::vector<raylib::Color>> pixels;
        int resolution_x, resolution_y;

    public:
        Canvas(int resolution_x, int resolution_y, raylib::Vector3 pos)
            : position(pos), resolution_x(resolution_x), resolution_y(resolution_y)
        {
            pixels = std::vector(resolution_y, std::vector(resolution_x, raylib::Color::Black()));
        }

        ~Canvas() {}

        // Given (0,0), should return the top left pixel
        // AKA (position.x - resx/2, position.y - resy/2)

        // Given (resx,resy), should return the bottom right pixel
        // AKA (position + resx/2, position+resy/2)

        // Given (resx/2, resy/2), should return the center
        // AKA (position.x, position.y)
        raylib::Vector3 GetRayDirection(int x, int y, raylib::Vector3 camFwd)
        {
            return camFwd +
                   raylib::Vector3(-resolution_x / 2, resolution_y / 2, 0) + // point vector to top left
                   raylib::Vector3(x, -y);                                   // point vector to correct position
        }
    };
}