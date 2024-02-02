#pragma once

#include <fstream>

#include "common.h"

using color = vec3;

void write_color(std::ofstream& output_file, color pixel_color, int samples_per_pixel)
{
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0, 255] value of each color component.
    static const interval intensity(0.000, 0.999);
    output_file << static_cast<int>(256 * intensity.clamp(r)) << ' '
                << static_cast<int>(256 * intensity.clamp(g)) << ' '
                << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}
