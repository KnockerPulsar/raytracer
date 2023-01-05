#include "raylib.h"

#include <cstdint>
#include <iostream>

#define u8 uint8_t 

void drawPixel(
		u8* buffer,
		int bufferWidth,
		int bufferHeight,
		int x,
		int y,
		u8 r,
		u8 g,
		u8 b) {

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

		buffer[index + 0] = r;
		buffer[index + 1] = g;
		buffer[index + 2] = b;
		buffer[index + 3] = 255;
}

int main() {
	const int windowWidth = 600;
	const int windowHeight = 600;

	InitWindow(windowWidth, windowHeight, "Rayborn");

	RenderTexture2D renderTexture = LoadRenderTexture(windowWidth, windowHeight);
	uint8_t* pixels = new uint8_t[windowWidth * windowHeight * 4];
	

	while(!WindowShouldClose()) {
		for (int j = 0; j < windowHeight; j++) {
			for (int i = 0; i < windowWidth; i++) {
				float u = float(i) / (windowWidth - 1);
				float v = float(j) / (windowHeight - 1);
				u8 r = u * 255.999;
				u8 g = v * 255.999;
				u8 b = 0.25f * 255.999;

				drawPixel(pixels, windowWidth, windowHeight, i, j, r, g, b);
			}
		}


		BeginDrawing();

		UpdateTexture(renderTexture.texture, pixels);

		DrawTextureRec(
				renderTexture.texture, 
				(Rectangle){0, 0, float(windowWidth), float(windowHeight) }, 
				(Vector2) {0, 0}, 
				WHITE
		);

		EndDrawing();
	}

	CloseWindow();
}
