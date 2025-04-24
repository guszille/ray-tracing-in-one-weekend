#pragma once

#include "common.h"

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "thread_pool.h"

#include "external/stbi/stb_image_write.h"

class camera
{
public:
	double aspect_ratio = 1.0; // Ratio of image, width over height.
	int image_width = 100; // Rendered image width in pixel count.
	int samples_per_pixel = 10; // Count of random samples for each pixel.
	int max_depth = 10; // Maximum number of ray bounces into scene.

	double vfov = 90.0; // Vertical view angle (field of view).
	point3 lookfrom = point3(0.0, 0.0, -1.0); // Point camera is looking from.
	point3 lookat = point3(0.0, 0.0, 0.0); // Point camera is looking at.
	vec3 vup = vec3(0.0, 1.0, 0.0); // Camera-relative "up" direction.

	double defocus_angle = 0; // Variation angle of rays through each pixel.
	double focus_distance = 10; // Distance from camera lookfrom point to plane of perfect focus.

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

	void render(const hittable& world, const char* filename)
	{
		initialize();

		unsigned char* buffer = new unsigned char[image_height * image_width * 3];
		uint32_t completed_tasks = 0;
		thread_pool tp;

		for (int j = 0; j < image_height; ++j)
		{
			for (int i = 0; i < image_width; ++i)
			{
				tp.enqueue([&, i, j] {
					int stride = (j * image_width + i) * 3;
					color pixel_color(0.0, 0.0, 0.0);

					for (int sample = 0; sample < samples_per_pixel; ++sample)
					{
						ray r = get_ray(i, j);
						pixel_color += get_ray_color(r, max_depth, world);
					}

					write_color(buffer, stride, pixel_color, samples_per_pixel);
				});
			}
		}

		tp.terminate();

		stbi_write_jpg(filename, image_width, image_height, 3, buffer, 100);

		delete[] buffer;
	}

private:
	int image_height; // Rendered image height.
	point3 center; // Camera center.
	point3 pixel00_loc;	// Location of pixel (0, 0).
	vec3 pixel_delta_u; // Offset to pixel to the right.
	vec3 pixel_delta_v; // Offset to pixel below.
	vec3 u, v, w; // Camera frame basis vectors.
	vec3 defocus_disk_u; // Defocus disk horizontal radius.
	vec3 defocus_disk_v; // Defocus disk vertical radius.

	void initialize()
	{
		image_height = std::max(static_cast<int>(image_width / aspect_ratio), 1);

		center = lookfrom;

		// Determine viewport dimensions.
		double frustrum_height = tan(degrees_to_radians(vfov) / 2);
		double viewport_height = 2.0 * focus_distance * frustrum_height;
		double actual_aspect_ratio = static_cast<double>(image_width) / static_cast<double>(image_height);
		double viewport_width = viewport_height * actual_aspect_ratio;

		// Calculate the "u, v, w" unit basis vectors for the camera coordinate frame.
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		// Calculate the vectors across the horizontal and down the vertical viewport edges.
		vec3 viewport_u = viewport_width * u; // Vector across viewport horizontal edge.
		vec3 viewport_v = viewport_height * -v; // Vector down viewport vertical edge.

		// Calculate the horizontal and vertical delta vectors from pixel to pixel.
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Calculate the location of the upper left pixel.
		vec3 viewport_upper_left = center - (focus_distance * w) - viewport_u / 2.0 - viewport_v / 2.0;

		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		// Calculate the camera defocus disk basis vectors.
		double defocus_radius = focus_distance * tan(degrees_to_radians(defocus_angle / 2.0));

		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	ray get_ray(int i, int j) const
	{
		// Get a randomly sampled camera ray for the pixel at location (i, j), originating
		// from the camera defocus disk.

		point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
		point3 pixel_sample = pixel_center + pixel_sample_square();

		point3 ray_origin = (defocus_angle <= 0.0) ? center : defocus_disk_sample();;
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

	point3 defocus_disk_sample() const
	{
		vec3 p = random_in_unit_disk();

		// Returns a random point in the camera defocus disk.
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
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
			color attenuation;
			ray scattered;

			if (rec.mat->scatter(r, rec, attenuation, scattered))
			{
				return attenuation * get_ray_color(scattered, depth - 1, world);
			}

			return color(0.0, 0.0, 0.0);
		}

		vec3 unit_direction = unit_vector(r.get_direction());
		double a = 0.5 * (unit_direction.y() + 1.0);

		return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
	}
};
