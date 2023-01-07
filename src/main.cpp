#include "hittable.h"
#include "raytracer.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

#include "raylib.h"
#include "vec3.h"
#include "material.h"
#include <cstdint>
#include <iostream>

void updateFramebuffer(Texture tex, u8* pixels, Rectangle sourceRect, Rectangle targetRect) {
		BeginDrawing();

		UpdateTexture(tex, pixels);

		DrawTexturePro(
				tex, 
				sourceRect,
				targetRect,
				Vector2 {0, 0}, 
				0, 
				WHITE
		);

		EndDrawing();
}

inline tuple<u8, u8, u8> toRGB8(const vec3& v, int samplesPerPixel = 1) {
	vec3 c = v;

#ifdef GAMMA_CORRECTION
	float scale = 1.0f / samplesPerPixel;
	c[0] = sqrt(scale * c[0]);
	c[1] = sqrt(scale * c[1]);
	c[2] = sqrt(scale * c[2]);
#endif	

	static const interval intensity(0, 0.999f);
	c[0] = intensity.clamp(c[0]);
	c[1] = intensity.clamp(c[1]);
	c[2] = intensity.clamp(c[2]);
	
	return {
		u8(c[0] * 256),
		u8(c[1] * 256),
		u8(c[2] * 256)
	};
}

color rayColor(const ray& r, const hittable_list& world, int depth) {
	hit_record rec;

	if(depth <= 0) return color(0, 0, 0);

	if(world.hit(r, interval(0.001, infinity), rec)) {
		ray scattered;
		color attenutaion;

		if(rec.mat->scatter(r, rec, attenutaion, scattered)) {
            return attenutaion * rayColor(scattered, world, depth - 1);
		}

		return color(0, 0, 0);
	}

	vec3 unitDirection = unitVector(r.direction());

	float a = 0.5 * (unitDirection.y() + 1.0f);
	return (1.f - a) * color(1.f, 1.f, 1.f) + a * color(0.5f, 0.7f, 1.f);
}

void drawPixel(
		u8* buffer,
		int bufferWidth,
		int bufferHeight,
		int samplesPerPixel,
		int x,
		int y,
		color c
) {

	if(x < 0 || x >= bufferWidth || y < 0 || y >= bufferHeight) {
		TraceLog(
				LOG_ERROR,
				"X or Y outside of buffer bounds. Buffer width: %d, buffer height: %d, x: %d, y: %d",
				bufferWidth,
				bufferHeight,
				x,
				y
		);
	}

	// Assuming a R8G8B8A8 buffer.
	// Raylib's default render textures have this format.
	const int stride = 4;
	int index = (bufferWidth * y + x) * stride;
	auto [r, g, b] = toRGB8(c, samplesPerPixel);

	buffer[index + 0] = r;
	buffer[index + 1] = g;
	buffer[index + 2] = b;
	buffer[index + 3] = 255;
}

int main() {
	int numFrames = 1;

	const int imageWidth = 640;
	const int imageHeight = 360;

	const int windowWidth = 1280;
	const int windowHeight = 720;

	const float aspectRatio = float(imageWidth) / imageHeight;
	const int samplesPerPixel = 10;
	const int maxDepth = 12;

	hittable_list world;
	auto materialGround = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto materialCenter = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	auto materialLeft = make_shared<dielectric>(1.5);
	auto materialRight = make_shared<metal>(color(0.8, 0.6, 0.2), 0);


	world.add(make_shared<sphere>(point3( 0, -100.5, -1), 100, materialGround));
	world.add(make_shared<sphere>(point3( 0, 			0, -1), 0.5, materialCenter));
	world.add(make_shared<sphere>(point3(-1, 			0, -1), 0.5, materialLeft));
	world.add(make_shared<sphere>(point3(-1, 			0, -1), -0.4, materialLeft));
	world.add(make_shared<sphere>(point3( 1, 			0, -1), 0.5, materialRight));

	camera cam(aspectRatio);

	InitWindow(windowWidth, windowHeight, "Rayborn");

	RenderTexture2D renderTexture = LoadRenderTexture(imageWidth, imageHeight);
	u8* pixels = new uint8_t[imageWidth * imageHeight * 4];


	while(!WindowShouldClose()) {
		if(numFrames > 0) {
			for (int j = 0; j < imageHeight; j++) {

				std::clog << "\rScanlines remaining " << imageHeight - j << ' ' << std::flush;

				for (int i = 0; i < imageWidth; i++) {
					color pixelColor(0, 0, 0);

					for (int sample = 0; sample < samplesPerPixel; sample++) {
						float s = (i + randomFloat()) / (imageWidth - 1);
						float t = (j + randomFloat()) / (imageHeight - 1);

						ray r = cam.getRay(s, t);
						pixelColor += rayColor(r, world, maxDepth);
					}

					drawPixel(pixels, imageWidth, imageHeight, samplesPerPixel, i, j, pixelColor);
				}

				updateFramebuffer(
					renderTexture.texture,
					pixels,
					(Rectangle){0, 0, float(imageWidth), float(imageHeight)},
					(Rectangle){0, 0, float(windowWidth), float(windowHeight)}
				);
			}

			numFrames--;
			std::clog << "\rDone.                \n" << std::flush;
		} else {
				updateFramebuffer(
					renderTexture.texture,
					pixels,
					(Rectangle){0, 0, float(imageWidth), float(imageHeight)},
					(Rectangle){0, 0, float(windowWidth), float(windowHeight)}
				);
		}
	}

	CloseWindow();
}
