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
#include <memory>

int main() {
	scene sceneDesc;

	sceneDesc.windowWidth = 400;
	sceneDesc.windowHeight = 225;
	sceneDesc.renderScale = 1.0f;
	sceneDesc.samplesPerPixel = 100;

	sceneDesc.cam.lookfrom = point3(-2, 2, 1);
	sceneDesc.cam.lookat 	 = point3(0, 0, -1);
	sceneDesc.cam.vup 		 = point3(0, 1, 0);
	sceneDesc.cam.vFov 		 = 20;

	hittable_list& world = sceneDesc.world;

	auto materialGround = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto materialCenter = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto materialLeft 	= make_shared<dielectric>(1.5);
	auto materialRight  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);


	world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, materialGround));
	world.add(make_shared<sphere>(point3( 0.0, 		0.0, -1.0),   0.5, materialCenter));
	world.add(make_shared<sphere>(point3(-1.0, 		0.0, -1.0),   0.5, materialLeft));
	world.add(make_shared<sphere>(point3(-1.0, 		0.0, -1.0),  -0.45, materialLeft));
 	world.add(make_shared<sphere>(point3( 1.0, 		0.0, -1.0),   0.5, materialRight));
 
	InitWindow(sceneDesc.windowWidth, sceneDesc.windowHeight, "Rayborn");

	sceneDesc.initFramebuffer();
	while(!WindowShouldClose()) {
		sceneDesc.render();
	}

	CloseWindow();
}
