#pragma once

#include "interval.h"
#include "perlin.h"
#include "raytracer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "rtw_stb_image.h"
#include "vec3.h"

class texture {
	public:
		virtual ~texture() = default;
		virtual color value(float u, float v, const point3& p) const = 0;
};

class solid_color : public texture {
	public:
		solid_color() {}
		solid_color(color c): color_value(c) {}

		solid_color(float r, float g, float b): solid_color(color(r, g, b)) {}

		color value(float u, float v, const point3& p) const override {
			return color_value;
		}

	private:
		color color_value;
};

class checker_texture: public texture {
	public:
		float inv_scale;
		std::shared_ptr<texture> even, odd;

	public:
		checker_texture() {}
		checker_texture(float _scale, std::shared_ptr<texture> _even, std::shared_ptr<texture> _odd)
			: inv_scale(1.0f / _scale), even(_even), odd(_odd) {}

		checker_texture(float _scale, color _even, color _odd)
			: inv_scale(1.0f / _scale), 
				even(std::make_shared<solid_color>(_even)), 
				odd(std::make_shared<solid_color>(_odd)) 
		{}

		color value(float u, float v, const point3& p) const override {
			auto x_int = static_cast<int>(std::floor(inv_scale * p.x()));
			auto y_int = static_cast<int>(std::floor(inv_scale * p.y()));
			auto z_int = static_cast<int>(std::floor(inv_scale * p.z()));

			bool is_even = (x_int + y_int + z_int) % 2 == 0;

			return is_even? even->value(u, v, p) : odd->value(u, v, p);
		}
};

class image_texture: public texture {
	public:
		image_texture() {}
		image_texture(const char* filename): image(filename) {}

		color value(float u, float v, const point3& p) const override {
			if (image.height() <= 0) return color(0, 1, 1);

			u = interval(0, 1).clamp(u);	
			v = 1.0f - interval(0, 1).clamp(v);	

			auto i = static_cast<int>(u * image.width());
			auto j = static_cast<int>(v * image.height());
			auto pixel = image.pixel_data(i, j);

			const auto color_scale = 1.0f / 255.0f;
			return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
		}
	private:
		rtw_image image;
};

class noise_texture: public texture {
	public:
		perlin noise;
		float scale;

	public:
		noise_texture() {}
		noise_texture(float sc): scale(sc) {}

		color value(float u, float v, const point3& p) const override {
			auto s  = scale * p;
			return color(1, 1, 1) * 0.5 * (1 + sinf(s.z() + 10 * noise.turb(s)));
		}
};
