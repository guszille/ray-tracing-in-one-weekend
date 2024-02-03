#pragma once

#include "common.h"
#include "color.h"
#include "hittable.h"

class camera
{
public:
	double aspect_ratio = 1.0; // Ratio of image, width over height.
	int image_width = 100; // Rendered image width in pixel count.
	int samples_per_pixel = 10; // Count of random samples for each pixel.
	int max_depth = 10; // Maximum number of ray bounces into scene.

	void render(const hittable& world, std::ofstream& output_file)
	{
		initialize();

		output_file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = 0; j < image_height; ++j)
		{
			std::clog << '\r' << "Lines remaining: " << (image_height - j) << ' ' << std::flush;

			for (int i = 0; i < image_width; ++i)
			{
				color pixel_color(0.0, 0.0, 0.0);

				for (int sample = 0; sample < samples_per_pixel; ++sample)
				{
					ray r = get_ray(i, j);
					pixel_color += get_ray_color(r, max_depth, world);
				}

				write_color(output_file, pixel_color, samples_per_pixel);
			}
		}

		std::clog << '\r' << "Done." << "                " << std::endl;
	}

private:
	int image_height; // Rendered image height.
	point3 center; // Camera center.
	point3 pixel00_loc;	// Location of pixel (0, 0).
	vec3 pixel_delta_u; // Offset to pixel to the right.
	vec3 pixel_delta_v; // Offset to pixel below.

	void initialize()
	{
		image_height = std::max(static_cast<int>(image_width / aspect_ratio), 1);

		// Determine viewport dimensions.
		double focal_length = 1.0;
		double viewport_height = 2.0;
		double actual_aspect_ratio = static_cast<double>(image_width) / static_cast<double>(image_height);
		double viewport_width = viewport_height * actual_aspect_ratio;
		
		center = point3(0.0, 0.0, 0.0);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		vec3 viewport_u = vec3(viewport_width, 0, 0);
		vec3 viewport_v = vec3(0, -viewport_height, 0);

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calculate the location of the upper left pixel.
		vec3 viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
	}

	ray get_ray(int i, int j) const
	{
		// Get a randomly sampled camera ray for the pixel at location (i, j).
		point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
		point3 pixel_sample = pixel_center + pixel_sample_square();

		point3 ray_origin = center;
		vec3 ray_direction = pixel_sample - ray_origin;

		return ray(ray_origin, ray_direction);
	}

	vec3 pixel_sample_square() const
	{
		double px = -0.5 + random_double();
		double py = -0.5 + random_double();

		// Returns a random point in the square surrounding a pixel at the origin.
		return (px * pixel_delta_u) + (py * pixel_delta_v);
	}

	color get_ray_color(const ray& r, int depth, const hittable& world) const
	{
		hit_record rec;

		// If we've exceeded the ray bounce limit, no more light is gathered.
		if (depth <= 0)
		{
			return color(0.0, 0.0, 0.0);
		}

		// Using "0.001" as the minimum value to avoid shadow acne.
		if (world.hit(r, interval(0.001, infinity), rec))
		{
			vec3 direction = rec.normal + random_unit_vector();

			return 0.5 * get_ray_color(ray(rec.p, direction), depth - 1, world);
		}

		vec3 unit_direction = unit_vector(r.get_direction());
		double a = 0.5 * (unit_direction.y() + 1.0);

		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}
};
