#pragma once

#include "common.h"
#include "hittable_list.h"
#include "texture.h"

class hit_record;

class material
{
public:
	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(const color& _albedo) : tex(std::make_shared<solid_color>(_albedo)) {}
	lambertian(std::shared_ptr<texture> _tex) : tex(_tex) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 scatter_direction = rec.normal + random_unit_vector();

		// Catch degenerate scatter direction.
		if (scatter_direction.near_zero())
		{
			scatter_direction = rec.normal;
		}

		attenuation = tex->value(rec.u, rec.v, rec.p);
		scattered = ray(rec.p, scatter_direction, r_in.get_time());

		return true;
	}

private:
	std::shared_ptr<texture> tex;
};

class metal : public material
{
public:
	metal(const color& _albedo, double _fuzz) : albedo(_albedo), fuzz(std::min(_fuzz, 1.0)) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(unit_vector(r_in.get_direction()), rec.normal);
		vec3 scatter_direction = reflected + (fuzz * random_unit_vector());

		attenuation = albedo;
		scattered = ray(rec.p, scatter_direction, r_in.get_time());

		return dot(scattered.get_direction(), rec.normal) > 0;
	}

private:
	color albedo;
	double fuzz;
};

class dielectric : public material
{
public:
	dielectric(double _refraction_index) : refraction_index(_refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override
	{
		vec3 scatter_direction;
		vec3 unit_direction = unit_vector(r_in.get_direction());

		double refraction_ratio = rec.front_face ? (1.0 / refraction_index) : refraction_index;
		double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = false;
		cannot_refract |= refraction_ratio * sin_theta > 1.0;
		cannot_refract |= reflectance(cos_theta, refraction_ratio) > random_double();

		if (cannot_refract)
		{
			scatter_direction = reflect(unit_direction, rec.normal);
		}
		else
		{
			scatter_direction = refract(unit_direction, rec.normal, refraction_ratio);
		}

		attenuation = color(1.0, 1.0, 1.0);
		scattered = ray(rec.p, scatter_direction, r_in.get_time());

		return true;
	}

private:
	double refraction_index;

	static double reflectance(double cosine, double refraction_index)
	{
		// Use Schlick's approximation for reflectance.
		double r0 = (1 - refraction_index) / (1 + refraction_index);
		r0 = r0 * r0;

		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
};
