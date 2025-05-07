#pragma once

#include <cmath>
#include <cstdlib>
#include <memory>
#include <limits>

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees)
{
	return degrees * pi / 180.0;
}

inline double random_double()
{
	return rand() / (RAND_MAX + 1.0); // Returns a random real in [0, 1).
}

inline double random_double(double min, double max)
{
	return min + (max - min) * random_double(); // Returns a random real in [min, max).
}

// Headers
#include "vec3.h"
#include "ray.h"
#include "interval.h"
