#pragma once

class interval
{
public:
	double min, max;

	interval() : min(+infinity), max(-infinity) {} // Default interval is empty.

	interval(double _min, double _max) : min(_min), max(_max) {}

	interval(const interval& a, const interval& b)
	{
		// Create the interval tightly enclosing the two input intervals.
		min = a.min <= b.min ? a.min : b.min;
		max = a.max >= b.max ? a.max : b.max;
	}

	double size() const
	{
		return max - min;
	}

	bool contains(double x) const
	{
		return min <= x && x <= max;
	}

	bool surrounds(double x) const
	{
		return min < x && x < max;
	}

	double clamp(double x) const
	{
		if (x < min) return min;
		if (x > max) return max;

		return x;
	}

	interval expand(double delta) const
	{
		double padding = delta / 2;

		return interval(min - padding, max + padding);
	}

	static const interval empty, universe;
};

const interval interval::empty(+infinity, -infinity);
const interval interval::universe(-infinity, +infinity);
