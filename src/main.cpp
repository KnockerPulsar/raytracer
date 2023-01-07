#include "hittable.h"
#include "raytracer.h"
#include "hittable_list.h"
#include "scene.h"
#include "sphere.h"
#include "camera.h"

#include "raylib.h"
#include "vec3.h"
#include "material.h"
#include <cstdint>
#include <iostream>

int main() {
	scene sceneDesc;

	sceneDesc.windowWidth = 640;
	sceneDesc.windowHeight = 640;
	sceneDesc.renderScale = 2.0f;
	sceneDesc.samplesPerPixel = 100;

	hittable_list& world = sceneDesc.world;
	auto materialGround = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto materialCenter = make_shared<lambertian>(color(0.7, 0.3, 0.3));
	auto materialLeft = make_shared<dielectric>(1.5);
	auto materialRight = make_shared<metal>(color(0.8, 0.6, 0.2), 0);


	world.add(make_shared<sphere>(point3( 0, -100.5, -1), 100, materialGround));
	world.add(make_shared<sphere>(point3( 0, 			0, -1), 0.5, materialCenter));
	world.add(make_shared<sphere>(point3(-1, 			0, -1), 0.5, materialLeft));
	world.add(make_shared<sphere>(point3(-1, 			0, -1), -0.4, materialLeft));
	world.add(make_shared<sphere>(point3( 1, 			0, -1), 0.5, materialRight));

	InitWindow(sceneDesc.windowWidth, sceneDesc.windowHeight, "Rayborn");

	sceneDesc.initFramebuffer();
	while(!WindowShouldClose()) {
		sceneDesc.render();
	}

	CloseWindow();
}
