#pragma once

#include <fstream>

#include "vec3.h"

using color = vec3;

void write_color(std::ofstream& output_file, color pixel_color)
{
    // Write the translated [0,255] value of each color component.
    output_file << static_cast<int>(255.999 * pixel_color.x()) << ' '
                << static_cast<int>(255.999 * pixel_color.y()) << ' '
                << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}
