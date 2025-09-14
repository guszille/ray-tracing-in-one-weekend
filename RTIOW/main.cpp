// Ray Tracing In One Weekend, by Gustavo Zille.

#ifndef STBI_INCLUDED
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#define STBI_INCLUDED
#endif

#define _CRT_SECURE_NO_WARNINGS // FIXME.

#include "libs/common.h"
#include "libs/color.h"
#include "libs/sphere.h"
#include "libs/hittable.h"
#include "libs/hittable_list.h"
#include "libs/camera.h"
#include "libs/material.h"
#include "libs/bvh.h"

void bouncing_spheres()
{
	// World.
	hittable_list world;

	// auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
	// world.add(std::make_shared<sphere>(point3(0.0, -1000.0, 0.0), 1000.0, ground_material));

	auto checker = std::make_shared<checker_texture>(0.32, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	world.add(std::make_shared<sphere>(point3(0, -1000.0, 0), 1000.0, std::make_shared<lambertian>(checker)));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			double choose_material = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4.0, 0.2, 0.0)).length() > 0.9)
			{
				std::shared_ptr<material> sphere_material;

				if (choose_material < 0.8)
				{
					auto albedo = color::random() * color::random();
					auto secondary_center = center + vec3(0.0, random_double(0.0, 0.5), 0.0);

					// Diffuse.
					sphere_material = std::make_shared<lambertian>(albedo);
					world.add(std::make_shared<sphere>(center, secondary_center, 0.2, sphere_material));
				}
				else if (choose_material < 0.95)
				{
					auto albedo = color::random(0.5, 1.0);
					auto fuzz = random_double(0.0, 0.5);

					// Metal.
					sphere_material = std::make_shared<metal>(albedo, fuzz);
					world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
				}
				else
				{
					// Glass.
					sphere_material = std::make_shared<dielectric>(1.5);
					world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = std::make_shared<dielectric>(1.5);
	world.add(std::make_shared<sphere>(point3(0.0, 1.0, 0.0), 1.0, material1));

	auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(std::make_shared<sphere>(point3(-4.0, 1.0, 0.0), 1.0, material2));

	auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(std::make_shared<sphere>(point3(4.0, 1.0, 0.0), 1.0, material3));

	world = hittable_list(std::make_shared<bvh_node>(world));

	// Camera.
	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20.0;
	cam.lookfrom = point3(13.0, 2.0, 3.0);
	cam.lookat = point3(0.0, 0.0, 0.0);
	cam.vup = vec3(0.0, 1.0, 0.0);

	cam.defocus_angle = 0.6;
	cam.focus_distance = 10.0;

	cam.render_mt(world, "outputs/book2/image1.jpg");
}

void checkered_spheres()
{
	// World.
	hittable_list world;

	auto checker = std::make_shared<checker_texture>(0.32, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

	world.add(std::make_shared<sphere>(point3(0, -10, 0), 10, std::make_shared<lambertian>(checker)));
	world.add(std::make_shared<sphere>(point3(0, 10, 0), 10, std::make_shared<lambertian>(checker)));

	// Camera.
	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = point3(13, 2, 3);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0;

	cam.render(world, "outputs/book2/image2.jpg");
}

void earth()
{
	// World.
	hittable_list world;

	auto earth_texture = std::make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = std::make_shared<lambertian>(earth_texture);
	auto globe = std::make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

	world.add(globe);

	// Camera.
	camera cam;

	cam.aspect_ratio = 16.0 / 9.0;
	cam.image_width = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth = 50;

	cam.vfov = 20;
	cam.lookfrom = point3(0, 0, 12);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vec3(0, 1, 0);

	cam.defocus_angle = 0;

	cam.render(world, "outputs/book2/image3.jpg");
}

int main()
{
	// std::srand(std::time(NULL));

	switch (3)
	{
	case 1: bouncing_spheres(); break;
	case 2: checkered_spheres(); break;
	case 3: earth(); break;
	}

	return 0;
}
