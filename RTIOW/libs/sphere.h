#pragma once

#include "common.h"

#include "hittable.h"

class sphere : public hittable
{
public:
	sphere(const point3& _static_center, double _radius, std::shared_ptr<material> _mat) // Stationary sphere.
		: center(_static_center, vec3(0.0, 0.0, 0.0)), radius(std::fmax(0.0, _radius)), mat(_mat) {}

	sphere(const point3& _center1, const point3& _center2, double _radius, std::shared_ptr<material> _mat) // Moving sphere.
		: center(_center1, _center2 - _center1), radius(std::fmax(0.0, _radius)), mat(_mat) {}

	bool hit(const ray& r, interval ray_ti, hit_record& rec) const override
	{
		point3 current_center_position = center.at(r.get_time());
		vec3 oc = current_center_position - r.get_origin();

		double a = r.get_direction().length_squared();
		double half_b = dot(oc, r.get_direction());
		double c = oc.length_squared() - radius * radius;
		double discriminant = half_b * half_b - a * c;

		if (discriminant < 0) return false;

		double sqrtd = sqrt(discriminant);

		// Find the nearest root that lies in the acceptable range.
		double root = (half_b - sqrtd) / a;

		if (!ray_ti.surrounds(root))
		{
			root = (half_b + sqrtd) / a;

			if (!ray_ti.surrounds(root))
			{
				return false;
			}
		}

		rec.t = root;
		rec.p = r.at(rec.t);
		vec3 outward_normal = (rec.p - current_center_position) / radius;
		rec.set_face_normal(r, outward_normal);
		rec.mat = mat;

		return true;
	}

private:
	ray center;
	double radius;
	std::shared_ptr<material> mat;
};
