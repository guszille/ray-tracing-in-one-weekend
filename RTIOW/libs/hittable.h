#pragma once

#include "common.h"

class material;

class hit_record
{
public:
	point3 p;
	vec3 normal;
	std::shared_ptr<material> mat;
	double t;
	bool front_face;

	void set_face_normal(const ray& r, const vec3& outward_normal)
	{
		// Sets the hit record normal vector.
		// NOTE: the parameter "outward_normal" is assumed to have unit length.

		front_face = dot(r.get_direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable
{
public:
	virtual ~hittable() = default;

	virtual bool hit(const ray& r, interval ray_ti, hit_record& rec) const = 0;
};
