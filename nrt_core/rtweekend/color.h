#pragma once

#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

/*void write_color(std::ostream &out, color pixel_color, int samples_per_pixel)
{
	double r = isnan(pixel_color.x()) ? 0.0 : clamp(pixel_color.x(), 0.0, infinity);
	double g = isnan(pixel_color.y()) ? 0.0 : clamp(pixel_color.y(), 0.0, infinity);
	double b = isnan(pixel_color.z()) ? 0.0 : clamp(pixel_color.z(), 0.0, infinity);
	
	double scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	out << static_cast<int>(256 * clamp(r, 0.0, 0.99)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.99)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.99)) << '\n';
}*/

#endif