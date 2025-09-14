#pragma once

#include "common.h"
#include "hittable.h"

class sphere : public hittable
{
public:
	sphere(const point3& _static_center, double _radius, std::shared_ptr<material> _mat) // Stationary sphere.
		: center(_static_center, vec3(0.0, 0.0, 0.0)), radius(std::fmax(0.0, _radius)), mat(_mat)
	{
		vec3 rv = vec3(radius, radius, radius);

		bbox = aabb(_static_center - rv, _static_center + rv);
	}

	sphere(const point3& _center1, const point3& _center2, double _radius, std::shared_ptr<material> _mat) // Moving sphere.
		: center(_center1, _center2 - _center1), radius(std::fmax(0.0, _radius)), mat(_mat)
	{
		auto rv = vec3(radius, radius, radius);

		aabb box0(center.at(0) - rv, center.at(0) + rv);
		aabb box1(center.at(1) - rv, center.at(1) + rv);

		bbox = aabb(box0, box1);
	}

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
		get_sphere_uv(outward_normal, rec.u, rec.v);
		rec.mat = mat;

		return true;
	}

	aabb get_bounding_box() const override { return bbox; }

private:
	ray center;
	double radius;
	std::shared_ptr<material> mat;
	aabb bbox;

	static void get_sphere_uv(const point3& p, double& u, double& v)
	{
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		// 
		// < 1  0  0> yields <0.50 0.50>	<-1  0  0> yields <0.00 0.50>
		// < 0  1  0> yields <0.50 1.00>	< 0 -1  0> yields <0.50 0.00>
		// < 0  0  1> yields <0.25 0.50>	< 0  0 -1> yields <0.75 0.50>

		auto theta = std::acos(-p.y());
		auto phi = std::atan2(-p.z(), p.x()) + pi;

		u = phi / (2 * pi);
		v = theta / pi;
	}
};
