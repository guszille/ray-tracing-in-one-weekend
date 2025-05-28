#pragma once

#include "vec3.h"

class ray
{
public:
	ray() {}

	ray(const point3& _origin, const vec3& _direction, double _time)
		: origin(_origin), direction(_direction), time(_time) {}

	ray(const point3& _origin, const vec3& _direction)
		: origin(_origin), direction(_direction), time(0.0) {}

	const point3& get_origin() const { return origin; }
	const vec3& get_direction() const { return direction; }
	double get_time() const { return time; }

	point3 at(double t) const
	{
		return origin + t * direction;
	}

private:
	point3 origin;
	vec3 direction;
	double time;
};
