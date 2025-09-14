#pragma once

#include <memory>
#include <vector>

#include "common.h"
#include "hittable.h"
#include "aabb.h"

class hittable_list : public hittable
{
public:
	std::vector<std::shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(std::shared_ptr<hittable> object) { add(object); }

	void clear()
	{
		objects.clear();
	}

	void add(std::shared_ptr<hittable> object)
	{
		objects.push_back(object);

		bbox = aabb(bbox, object->get_bounding_box());
	}

	bool hit(const ray& r, interval ray_ti, hit_record& rec) const override
	{
		hit_record temp_rec;
		bool hit_anything = false;
		double closest_so_far = ray_ti.max;

		for (const auto& object : objects)
		{
			if (object->hit(r, interval(ray_ti.min, closest_so_far), temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}

		return hit_anything;
	}

	aabb get_bounding_box() const override { return bbox; }

private:
	aabb bbox;
};
