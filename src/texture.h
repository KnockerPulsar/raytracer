#pragma once

#include "raytracer.h"

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
