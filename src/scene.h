#pragma once

#include "raytracer.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include <raylib.h>

class scene {
	public:
		hittable_list world;
		camera cam;
		int imageWidth;
		int imageHeight;
		int samplesPerPixel = 10;
		int maxDepth = 50;

		RenderTexture2D renderTexture;
		u8* pixels = nullptr;
		int windowWidth = 1280;
		int windowHeight = 720;
		float renderScale = 0.5f;

		// To initialze the framebuffer for rendering to the screen.
		void initFramebuffer() {
			imageWidth = int(windowWidth * renderScale);
			imageHeight = int(windowHeight * renderScale);

			if(imageWidth <= 0 || imageHeight <= 0) {
				TraceLog(
					LOG_ERROR, 
					"Image width and height must be greater than zero. Current width %d, current height %d",
					imageWidth, imageHeight
				);
			}

			renderTexture = LoadRenderTexture(imageWidth, imageHeight);
			pixels = new uint8_t[imageWidth * imageHeight * 4];
		}

		void render() {
			cam.initialize(float(imageWidth) / imageHeight);

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
			std::clog << "\rDone.                \n" << std::flush;
		}

	private:
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

		// Functions to draw to raylib's window.

		static void updateFramebuffer(Texture tex, u8* pixels, Rectangle sourceRect, Rectangle targetRect) {
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

		static inline tuple<u8, u8, u8> toRGB8(const vec3& v, int samplesPerPixel = 1) {
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

		static void drawPixel(u8* buffer, int bufferWidth, int bufferHeight,
													int samplesPerPixel, int x, int y, color c) 
		{

			if(x < 0 || x >= bufferWidth || y < 0 || y >= bufferHeight) {
				TraceLog(
					LOG_ERROR, "X or Y outside of buffer bounds. Buffer width: %d, buffer height: %d, x: %d, y: %d",
					bufferWidth, bufferHeight, x, y
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
};