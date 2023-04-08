#include "bvh.h"
#include "hittable.h"
#include "moving_sphere.h"
#include "raytracer.h"
#include "hittable_list.h"
#include "scene.h"
#include "sphere.h"
#include "camera.h"
#include "texture.h"
#include "vec3.h"
#include "material.h"
#include "rtw_stb_image.h"
#include "quad.h"

#include "raylib.h"

#include <cstdint>
#include <iostream>
#include <memory>

void randomSpheres(scene& sceneDesc) {
	sceneDesc.windowWidth = 1280;
	sceneDesc.windowHeight = 720;
	sceneDesc.renderScale = 0.25f;
	sceneDesc.samplesPerPixel = 100;

	sceneDesc.cam.lookfrom = point3(13, 2, 3);
	sceneDesc.cam.lookat 	 = point3(0, 0, 0);
	sceneDesc.cam.vup 		 = point3(0, 1, 0);
	sceneDesc.cam.vFov 		 = 20;
	sceneDesc.cam.aperture = 0.1;
	sceneDesc.cam.focusDist = 10.0;

	hittable_list& world = sceneDesc.world;

	auto checker = make_shared<checker_texture>(0.66, color(.2, .3, .1), color(.9, .9, .9));
	auto groundMaterial = make_shared<lambertian>(checker);
	world.add(make_shared<sphere>(point3(0.0, -1000, -1.0), 1000, groundMaterial));

	const int width = 22;
	const int height = 22;
	for (int a = -width/2; a < width/2; a++) {
		for (int b = -height/2; b < height/2; b++) {
			float chooseMat = randomFloat();
			point3 center(a + 0.9 * randomFloat(), 0.2, b + 0.9 * randomFloat());

			if((center - point3(4, 0.2, 0)).length() > 0.9) {
				sPtr<material> sphereMaterial;

				if(chooseMat < 0.8) {
					// Diffuse
					color albedo = color::random() * color::random();
					sphereMaterial = make_shared<lambertian>(albedo);

					// Stationary sphere
					/* world.add(make_shared<sphere>(center, 0.2, sphereMaterial)); */

					// Moving sphere
					vec3 center2 = center + vec3(0, randomFloat(0, 0.5), 0);
					world.add(make_shared<moving_sphere>(center, center2, 0.2, sphereMaterial));
				} else if (chooseMat < 0.95) {
					// Metal
					color albedo = color::random(0.5, 1);
					float fuzz = randomFloat(0, 0.5);

					sphereMaterial = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
				} else {
					// Glass
					sphereMaterial = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));


	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

	sceneDesc.world = hittable_list(make_shared<bvh_node>(world));
} 

void two_spheres(scene& scene_desc) {
	scene_desc.windowWidth = 400;
	scene_desc.windowHeight = 225;

	scene_desc.samplesPerPixel = 100;

	scene_desc.cam.aperture = 0.0f;
	scene_desc.cam.vFov = 20.0f;

	scene_desc.cam.lookfrom = point3(13, 2, 3);
	scene_desc.cam.lookat = point3(0, 0, 0);

	hittable_list& world = scene_desc.world;
	auto checker = make_shared<lambertian>(make_shared<checker_texture>(0.8, color(.2, .3, .1), color(.9, .9, .9)));

	world.add(make_shared<sphere>(point3(0, -10, 0), 10, checker));
	world.add(make_shared<sphere>(point3(0,  10, 0), 10, checker));
}

void earth(scene& scene_desc) {
	scene_desc.windowWidth = 1280;
	scene_desc.windowHeight = 720;
	scene_desc.samplesPerPixel = 100;

	scene_desc.cam.aperture = 0;
	scene_desc.cam.vFov = 20;
	scene_desc.cam.lookfrom = point3(0, 0, 12);
	scene_desc.cam.lookat = point3(0, 0, 0);

	auto earth_texture = make_shared<image_texture>("earthmap.png");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

	scene_desc.world = hittable_list(globe);
}


void two_perlin_spheres(scene& scene_desc) {
	scene_desc.windowWidth = 1280;
	scene_desc.windowHeight = 720;

	scene_desc.samplesPerPixel = 100;

	scene_desc.cam.aperture = 0.0f;
	scene_desc.cam.vFov = 20.0f;

	scene_desc.cam.lookfrom = point3(13, 2, 3);
	scene_desc.cam.lookat = point3(0, 0, 0);

	hittable_list& world = scene_desc.world;
	auto pertext = make_shared<noise_texture>(4);
	auto permat = make_shared<lambertian>(pertext);

	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, permat));
	world.add(make_shared<sphere>(point3(0,  2, 0), 2, permat));
}

void quads(scene& scene_desc) {
	scene_desc.windowWidth = 1280;
	scene_desc.windowHeight = 720;
	scene_desc.samplesPerPixel = 100;


	scene_desc.cam.aperture = 0.0;
	scene_desc.cam.vFov = 80.0;
	scene_desc.cam.lookfrom = point3(0,0,9);
	scene_desc.cam.lookat = point3(0,0,0);

	hittable_list& world = scene_desc.world;

	// Materials
	auto left_red = make_shared<lambertian>(color(1,.2,.2));
	auto back_green = make_shared<lambertian>(color(.2,1,.2));
	auto right_blue = make_shared<lambertian>(color(.2,.2,1));
	auto upper_orange = make_shared<lambertian>(color(1,.5,0));
	auto lower_teal = make_shared<lambertian>(color(.2,.8,.8));

	// Quads
	world.add(make_shared<quad>(point3(-3,-2,5), vec3(0,0,-4), vec3(0,4,0), left_red));
	world.add(make_shared<quad>(point3(-2,-2,0), vec3(4,0,0), vec3(0,4,0), back_green));
	world.add(make_shared<quad>(point3(3,-2,1), vec3(0,0,4), vec3(0,4,0), right_blue));
	world.add(make_shared<quad>(point3(-2,3,1), vec3(4,0,0), vec3(0,0,4), upper_orange));
	world.add(make_shared<quad>(point3(-2,-3,5), vec3(4,0,0), vec3(0,0,-4), lower_teal));
}

void simple_light(scene& scene_desc) {
    scene_desc.windowWidth = 1280;
    scene_desc.windowHeight = 720;
		scene_desc.renderScale = 0.5f;
    scene_desc.samplesPerPixel = 1000;
    scene_desc.background = color(0,0,0);

    scene_desc.cam.aperture = 0.0;
    scene_desc.cam.vFov = 20.0;
    scene_desc.cam.lookfrom = point3(26,3,6);
    scene_desc.cam.lookat = point3(0,2,0);

    hittable_list& world = scene_desc.world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    world.add(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));
}

void cornell_box(scene& scene_desc) {
    scene_desc.windowWidth = 720;
    scene_desc.windowHeight = 720;
		scene_desc.renderScale = 0.5f;
    scene_desc.samplesPerPixel = 1000;
    scene_desc.background        = color(0,0,0);

    scene_desc.cam.lookfrom = point3(278, 278, -800);
    scene_desc.cam.lookat   = point3(278, 278, 0);
    scene_desc.cam.vFov     = 40.0;
    scene_desc.cam.aperture = 0.0;

    hittable_list& world = scene_desc.world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));
}

int main() {
	scene sceneDesc;
	sceneDesc.background = color(.7, .8, 1.);
	sceneDesc.cam.vup = vec3(0, 1, 0);
	sceneDesc.cam.focusDist = 10;
	sceneDesc.renderScale = 1.0f;

	switch (0) {
		case 1: randomSpheres(sceneDesc);      break;
		case 2: two_spheres(sceneDesc);        break;
		case 3: earth(sceneDesc);              break;
		case 4: two_perlin_spheres(sceneDesc); break;
		case 5: quads(sceneDesc);              break;
		case 6: simple_light(sceneDesc); 			 break;
		default:
		case 7: cornell_box(sceneDesc); 			 break;
	}
 
	InitWindow(sceneDesc.windowWidth, sceneDesc.windowHeight, "Rayborn");


	auto finalScale = sceneDesc.renderScale;

	// Render once at quarter res
	// sceneDesc.renderScale = finalScale / 4.0f;
	// sceneDesc.initFramebuffer();
	// sceneDesc.render();

	// Render again at full res
	sceneDesc.renderScale = finalScale;
	sceneDesc.initFramebuffer();
	sceneDesc.render();

	sceneDesc.writeImage("./moving_spheres.ppm");

	CloseWindow();
}
