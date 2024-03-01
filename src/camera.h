#pragma once

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"

class camera {
  public:
    double aspect_ratio      = 1.0;
    int    image_width       = 100;
    int    samples_per_pixel = 10;
    int    max_depth         = 10;

    double vfov     = 90;
    point3 lookfrom = point3(0, 0, -1);
    point3 lookat   = point3(0, 0, 0);
    vec3   vup      = vec3(0, 1, 0);

    double defocus_angle = 0; // Variation angle of rays through each pixel
    double focus_dist = 10; // Distance from camera lookfrom point to the plane of perfect focus

    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n" << std::flush;
    }

  private:
    int    image_height; // Rendered image height
    point3 center;
    point3 pixel00_loc;   // Location of pixel 0, 0 (in world space)
    vec3   pixel_delta_u; // Offset to the pixel to the right
    vec3   pixel_delta_v; // Offset to the pixel below
    vec3   u, v, w;
    vec3 defocus_disk_u;  // Defocus disk horizontal radius
    vec3 defocus_disk_v;  // Defocus disk vertical radius
 

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Determine viewport dimensions
        auto theta           = degrees_to_radians(vfov);
        auto h               = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width =
            viewport_height * (static_cast<double>(image_width) / image_height);

        auto w = unit_vector(lookfrom - lookat);
        auto u = unit_vector(cross(vup, w));
        auto v = unit_vector(cross(w, u));

        // Calculate the vectors across the horizontal and down the vertical viewport edges
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;

        // Calculate the horizontal and vertical pixel delta
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel
        auto viewport_upper_left =
            center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

	auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
	defocus_disk_u = u * defocus_radius;
	defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
	// Note: Rays originate from the camera defocus disk
        auto pixel_center = pixel00_loc + i * (pixel_delta_u) + j * (pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin    = (defocus_angle <= 0)? center: defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
	auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 pixel_sample_square() const {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();

        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    point3 defocus_disk_sample() const {
	auto p = random_in_unit_disk();
	return lookfrom + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        // No more light is generated
        if (depth <= 0) return color(0, 0, 0);

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray   scattered;
            color attenuation;

            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, depth - 1, world);
            }

            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a              = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};
