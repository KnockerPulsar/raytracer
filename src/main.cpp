#include "bvh.h"
#include "constant_medium.h"
#include "hittable.h"
#include "moving_sphere.h"
#include "random.h"
#include "raytracer.h"
#include "hittable_list.h"
#include "scene.h"
#include "sphere.h"
#include "camera.h"
#include "texture.h"
#include "vec3.h"
#include "material.h"
#include "quad.h"

#include <memory>

void randomSpheres(scene& sceneDesc) {
	sceneDesc.windowWidth = 1920;
	sceneDesc.windowHeight = 1080;
	sceneDesc.renderScale = 1;
	sceneDesc.samplesPerPixel = 100;
	sceneDesc.maxDepth = 50;

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
		scene_desc.renderScale = 0.5;
    scene_desc.samplesPerPixel = 1000;
    scene_desc.background = color(0,0,0);

    scene_desc.cam.lookfrom = point3(278, 278, -800);
    scene_desc.cam.lookat = point3(278, 278, 0);
    scene_desc.cam.vFov = 40.0;
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

		std::shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
		box1 = make_shared<rotate_y>(box1, 15);
		box1 = make_shared<translate>(box1, vec3(265, 0, 295));
		world.add(box1);

		std::shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
		box2 = make_shared<rotate_y>(box2, -18);
		box2 = make_shared<translate>(box2, vec3(130, 0, 65));
		world.add(box2);
}

void cornell_smoke(scene& scene_desc) {
    scene_desc.windowWidth = 600;
    scene_desc.windowHeight = 600;
		scene_desc.renderScale = 1;
    scene_desc.samplesPerPixel = 1000;
    scene_desc.background  = color(0,0,0);

    scene_desc.cam.lookfrom = point3(278, 278, -800);
    scene_desc.cam.lookat = point3(278, 278, 0);
    scene_desc.cam.vFov = 40.0;
    scene_desc.cam.aperture = 0.0;

    hittable_list& world = scene_desc.world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(113,554,127), vec3(330,0,0), vec3(0,0,305), light));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

		std::shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
		box1 = make_shared<rotate_y>(box1, 15);
		box1 = make_shared<translate>(box1, vec3(265, 0, 295));

		std::shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
		box2 = make_shared<rotate_y>(box2, -18);
		box2 = make_shared<translate>(box2, vec3(130, 0, 65));
		
		world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
		world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

		scene_desc.world = hittable_list(make_shared<bvh_node>(world));
}

void final_scene(scene& scene_desc) {
    scene_desc.windowWidth = 800;
    scene_desc.windowHeight = 800;
    scene_desc.samplesPerPixel = 10000;
    scene_desc.renderScale = 0.1;
    scene_desc.background        = color(0,0,0);

    scene_desc.cam.aperture = 0.0;
    scene_desc.cam.vFov     = 40.0;
    scene_desc.cam.lookfrom = point3(478, 278, -600);
    scene_desc.cam.lookat   = point3(278, 278, 0);

    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

		const int boxes_per_side = 20;
		for (int i = 0; i < boxes_per_side; i++) {
			for (int j = 0; j < boxes_per_side; j++) {
				auto w = 100;
				auto x0 = -1000 + i * w;
				auto z0 = -1000 + j * w;
				auto y0 = 0.0;

				auto x1 = x0 + w;
				auto y1 = randomFloat(1, 101);
				auto z1 = z0 + w;

				boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
			}
		}

		hittable_list& world = scene_desc.world;

		world.add(make_shared<bvh_node>(boxes1));
	
		auto light = make_shared<diffuse_light>(color(7, 7, 7));
		world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

		auto center1 = point3(400, 400, 200);
		auto center2 = center1 + vec3(30, 0, 0);
		auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
		world.add(make_shared<moving_sphere>(center1, center2, 50, moving_sphere_material));

		world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
		world.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.8), 1.0)));

		auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
		world.add(boundary);
		world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
		boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
		world.add(make_shared<constant_medium>(boundary, 0.0001, color(1, 1, 1)));

		auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.png"));
		world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));

		auto pertext = make_shared<noise_texture>(0.1);
		world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

		hittable_list boxes2;
		auto white = make_shared<lambertian>(color(.73, .73, .73));
		int ns = 1000;
		for (int j = 0; j < ns; j++) {
			boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
		}

		world.add(make_shared<translate>( 
					make_shared<rotate_y>( make_shared<bvh_node>(boxes2), 15), 
					vec3(-100, 270, 395)
		));
}

void default_scene(scene& scene_desc) {
	final_scene(scene_desc);
	scene_desc.windowWidth = 400;
	scene_desc.windowHeight = 400;
	scene_desc.samplesPerPixel = 100;
	scene_desc.renderScale = 0.5;
	scene_desc.maxDepth = 4;
}

int main() {
	scene sceneDesc;
	sceneDesc.background = color(.7, .8, 1.);
	sceneDesc.cam.vup = vec3(0, 1, 0);
	sceneDesc.cam.focusDist = 10;
	sceneDesc.renderScale = 1.0f;

	switch (1) {
		case 1: randomSpheres(sceneDesc);      break;
		case 2: two_spheres(sceneDesc);        break;
		case 3: earth(sceneDesc);              break;
		case 4: two_perlin_spheres(sceneDesc); break;
		case 5: quads(sceneDesc);              break;
		case 6: simple_light(sceneDesc); 			 break;
		case 7: cornell_box(sceneDesc); 			 break;
		case 8: cornell_smoke(sceneDesc);      break;
		case 9: final_scene(sceneDesc);        break;
		default: default_scene(sceneDesc);     break;
	}
 

	auto finalScale = sceneDesc.renderScale;

	// Render again at full res
	sceneDesc.renderScale = finalScale;
	sceneDesc.initFramebuffer();
	sceneDesc.render();

	sceneDesc.writeImage("./moving_spheres.ppm");
}
