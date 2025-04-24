// Ray Tracing In One Weekend, by Gustavo Zille.

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include "libs/common.h"

#include "libs/color.h"
#include "libs/sphere.h"
#include "libs/hittable.h"
#include "libs/hittable_list.h"
#include "libs/camera.h"
#include "libs/material.h"

int main()
{
    // Image (output).
    std::ofstream image_file;

    image_file.open("outputs/image.ppm");

    // World.
    hittable_list world;

    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(point3(0.0, -1000.0, 0.0), 1000.0, ground_material));

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

                    // Diffuse.
                    sphere_material = std::make_shared<lambertian>(albedo);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
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

    // Camera.
    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1280;
    cam.samples_per_pixel = 150;
    cam.max_depth = 50;

    cam.vfov = 20.0;
    cam.lookfrom = point3(13.0, 2.0, 3.0);
    cam.lookat = point3(0.0, 0.0, 0.0);
    cam.vup = vec3(0.0, 1.0, 0.0);

    cam.defocus_angle = 0.6;
    cam.focus_distance = 10.0;

    cam.render(world, "outputs/image.jpg");

    image_file.close();

	return 0;
}
