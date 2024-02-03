// Ray tracing In One Weekend, by Gustavo Zille.

#include "libs/common.h"
#include "libs/color.h"
#include "libs/sphere.h"
#include "libs/hittable.h"
#include "libs/hittable_list.h"
#include "libs/camera.h"

int main()
{
    // Image (output).
    std::ofstream image_file;

    image_file.open("outputs/image.ppm");

    // World.
    hittable_list world;

    world.add(std::make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5));
    world.add(std::make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0));

    // Camera.
    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.render(world, image_file);

    image_file.close();

	return 0;
}
