// Ray tracing In One Weekend, by Gustavo Zille.

#include "libs/common.h"
#include "libs/color.h"
#include "libs/sphere.h"
#include "libs/hittable.h"
#include "libs/hittable_list.h"

color ray_color(const ray& r, const hittable& world)
{
    hit_record rec;

    if (world.hit(r, 0.0, infinity, rec))
    {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }

    vec3 unit_direction = unit_vector(r.get_direction());
    double a = 0.5 * (unit_direction.y() + 1.0);

    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main()
{
    // 1. File (output).
    std::ofstream image_file;
    image_file.open("outputs/image.ppm");

    // 2. Image.
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;

    // 2.1. Calculate the image height, and ensure that it's at least 1.
    int image_height = std::max(static_cast<int>(image_width / aspect_ratio), 1);

    // 3. Camera.
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double actual_aspect_ratio = static_cast<double>(image_width) / static_cast<double>(image_height);
    double viewport_width = viewport_height * actual_aspect_ratio;
    point3 camera_center = point3(0, 0, 0);

    // 3.1. Calculate the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // 3.2. Calculate the horizontal and vertical delta vectors from pixel to pixel.
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // 3.3. Calculate the location of the upper left pixel.
    vec3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    vec3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // 4. World.
    hittable_list world;

    world.add(std::make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5));
    world.add(std::make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0));

    // 5. Render.
    image_file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j)
    {
        std::clog << '\r' << "Lines remaining: " << (image_height - j) << ' ' << std::flush;

        for (int i = 0; i < image_width; ++i)
        {
            point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            vec3 ray_direction = pixel_center - camera_center;

            ray r(camera_center, ray_direction);
            color pixel_color = ray_color(r, world);
            
            write_color(image_file, pixel_color);
        }
    }

    std::clog << '\r' << "Done." << "                " << std::endl;

    image_file.close();

	return 0;
}
