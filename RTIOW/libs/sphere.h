#pragma once

#include "hittable.h"

class sphere : public hittable
{
public:
	sphere(point3 _center, double _radius) : center(_center), radius(_radius) {}

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override
    {
        vec3 oc = r.get_origin() - center;

        double a = r.get_direction().length_squared();
        double half_b = dot(oc, r.get_direction());
        double c = oc.length_squared() - radius * radius;
        double discriminant = half_b * half_b - a * c;

        if (discriminant < 0) return false;

        double sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        double root = (-half_b - sqrtd) / a;

        if (root <= ray_tmin || ray_tmax <= root)
        {
            root = (-half_b + sqrtd) / a;

            if (root <= ray_tmin || ray_tmax <= root)
            {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
	point3 center;
	double radius;
};