#pragma once

#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include <cstdlib>
#include <iostream>


// https://stackoverflow.com/a/874160
bool ends_with(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

class rtw_image {
	public:
		rtw_image(): data(nullptr) {}

		rtw_image(const char* image_filename) {
			auto filename = std::string(image_filename);

			if (ends_with(filename, ".jpg") || ends_with(filename, ".jpeg")) 
				std::cerr << "ERROR: jpg/jpeg textures are not supported (yet).\n"; 

			const auto imagedir = getenv("RTW_IMAGES");

			if (imagedir && load(std::string(imagedir) + "/" + image_filename)) return;
			if (load(filename)) return;

			std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
		}

		~rtw_image() { stbi_image_free(data); }

		bool load(const std::string filename) {
			auto n = bytes_per_pixel;

			// Since I'm currently linking with raylib, it includes its own version of STB.
			// There's one major difference: it doesn't support JPEG images, so we'll make do
			// with PNGs
			data = stbi_load(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
			bytes_per_scanline = image_width * bytes_per_pixel;

			return data != nullptr;
		}

		int width()  const { return (data == nullptr) ? 0 : image_width; }
		int height() const { return (data == nullptr) ? 0 : image_height; }

		const unsigned char* pixel_data(int x, int y) const {
			static unsigned char magenta[] = {255, 0, 255};
			if (data == nullptr) return magenta;

			x = clamp(x, 0, image_width);
			y = clamp(y, 0, image_height);

			return data + y * bytes_per_scanline + x * bytes_per_pixel;
		}
	private:
		const int bytes_per_pixel = 3;
		unsigned char * data;
		int image_width, image_height;
		int bytes_per_scanline;

		static int clamp(int x, int low, int high) {
			if (x < low) x = low;
			if (x < high) return x;
			return high - 1;
		}
};
