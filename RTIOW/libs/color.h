#pragma once

#include <fstream>

#include "vec3.h"

using color = vec3;

inline double linear_to_gamma(double linear_component)
{
    return (linear_component > 0) ? std::sqrt(linear_component) : 0.0;
}

void write_color_into_file(std::ofstream& output_file, color pixel_color)
{
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    // Apply the linear to gamma transform.
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Write the translated [0, 255] value of each color component.
    static const interval intensity(0.000, 0.999);
    output_file << static_cast<int>(256 * intensity.clamp(r)) << ' '
                << static_cast<int>(256 * intensity.clamp(g)) << ' '
                << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}

void write_color_into_buffer(unsigned char* buffer, int stride, color pixel_color)
{
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    // Apply the linear to gamma transform.
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Write the translated [0, 255] value of each color component.
    static const interval intensity(0.000, 0.999);
    buffer[stride + 0] = static_cast<int>(256 * intensity.clamp(r));
    buffer[stride + 1] = static_cast<int>(256 * intensity.clamp(g));
    buffer[stride + 2] = static_cast<int>(256 * intensity.clamp(b));
}
