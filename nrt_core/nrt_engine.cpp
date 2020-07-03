#include "nrt_engine.h"
#include "rtweekend/material.h"

void NRTEngine::set_camera(const vec3 &lookfrom, const vec3 &lookat, double vfov, double dist_to_focus, double aperture)
{
	cam = camera(lookfrom, lookat, vec3(0, 1, 0), vfov, aspect_ratio, aperture, dist_to_focus);

	is_camera_init = true;
}

void NRTEngine::set_params(int samples, int depth, const color &ground, const color &sky)
{
	samples_per_pixel = samples;
	max_depth = depth;

	ground_color = ground;
	sky_color = sky;
}

void NRTEngine::set_buffer(int in_full_width, int in_full_height, int in_corner_x, int in_corner_y, int in_width, int in_height)
{
	full_width = in_full_width;
	full_height = in_full_height;
	corner_x = in_corner_x;
	corner_y = in_corner_y;
	width = in_width;
	height = in_height;

	aspect_ratio = (float)full_width / full_height;

	pixels.resize(width * height * 3);
}

void NRTEngine::add_sphere(shared_ptr<sphere> &sph)
{
	world.add(sph);
}

void NRTEngine::clear()
{
	world.clear();
	pixels.clear();
	pixels.shrink_to_fit();
}

shared_ptr<material> NRTEngine::make_default_material()
{
	return make_shared<lambertian>(color(0.8, 0.8, 0.8));;
}

void NRTEngine::abort()
{
	abort_flag = true;
}

void NRTEngine::set_camera_aperture(double aperture)
{
	cam.set_aperture(aperture);
}

//-----------------------------------------------------------------------
//----------------------------main---------------------------------------

color NRTEngine::ray_color(const ray &r, const hittable &world, int depth)
{
	hit_record rec;
	if (depth <= 0)
	{
		return color(0, 0, 0);
	}

	if (world.hit(r, 0.01, infinity, rec))
	{
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * ray_color(scattered, world, depth - 1);
		}
		return color(0, 0, 0);
	}
	vec3 unit_direction = unit_vector(r.direction());
	double t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * ground_color + t * sky_color;
}

void NRTEngine::render()
{
	if (!is_rendering)
	{
		abort_flag = false;
	}
	if (is_camera_init)
	{
		is_rendering = true;
		int finish_steps = 0;
#pragma omp parallel for shared(finish_steps)
		for (int j = corner_y + height - 1; j >= corner_y; j--)
		{
			Tile tile(corner_x, j, width, 1);
			for (int i = corner_x; i < corner_x + width; i++)
			{
				color pixel_color(0, 0, 0);
				if (!abort_flag)
				{
					for (int s = 0; s < samples_per_pixel; s++)
					{
						double u = (i + random_double()) / (full_width - 1);
						double v = (j + random_double()) / (full_height - 1);
						ray r = cam.get_ray(u, v);
						pixel_color += ray_color(r, world, max_depth);
					}
					float r = (float)(pixel_color.x() / samples_per_pixel);
					float g = (float)(pixel_color.y() / samples_per_pixel);
					float b = (float)(pixel_color.z() / samples_per_pixel);

					tile.pixels[(i - corner_x) * 3] = r;
					tile.pixels[(i - corner_x) * 3 + 1] = g;
					tile.pixels[(i - corner_x) * 3 + 2] = b;

					//save pixel to global buffer
					pixels[((j - corner_y)*width + (i - corner_x)) * 3] = r;
					pixels[((j - corner_y)*width + (i - corner_x)) * 3 + 1] = g;
					pixels[((j - corner_y)*width + (i - corner_x)) * 3 + 2] = b;
				}
			}
			if (!abort_flag)
			{
				//progress_callback((double)(corner_y + height - 1 - j) / (height - 1));
				finish_steps++;
				progress_callback((double)(finish_steps) / (height - 1));
				update_tile_callback(tile);
			}
		}
		is_rendering = false;
		abort_flag = false;
	}
}