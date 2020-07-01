#pragma once
#include <functional>

#include "rtweekend/hittable_list.h"
#include "rtweekend/sphere.h"
#include "rtweekend/camera.h"
#include "rtweekend/material.h"

#include "tile.h"

#include <vector>

class NRTEngine
{
public:
	NRTEngine() 
	{
		is_camera_init = false;
		abort_flag = false;
		is_rendering = false;
		world.clear();
		pixels.resize(0);
	};
	~NRTEngine() 
	{
		pixels.clear();
		pixels.shrink_to_fit();
	};

	void set_buffer(int full_width, int full_height, int corner_x, int corner_y, int width, int height);
	void set_camera(const vec3 &lookfrom, const vec3 &lookat, double vfov, double dist_to_focus, double aperture);  // set camera AFTER buffer, because in the bufer we set aspect ratio
	void set_params(int samples, int depth, const color &ground, const color &sky);
	void add_sphere(shared_ptr<sphere> &sph);
	void clear();
	void render();
	void abort();
	shared_ptr<material> make_default_material();
	void set_camera_aperture(double aperture);

	std::vector<float> &get_buffer_pixels() { return pixels; }

	//callbacks
	void set_progress_callback(std::function<void(double progress)> callback) { progress_callback = callback; }
	void set_update_tile_callback(std::function<void(Tile &tile)> callback) { update_tile_callback = callback; }

private:
	color ray_color(const ray &r, const hittable &world, int depth);
	bool abort_flag;
	bool is_rendering;

	hittable_list world;
	camera cam;
	bool is_camera_init;

	int samples_per_pixel;
	int max_depth;
	color ground_color;
	color sky_color;
	float aspect_ratio;

	int full_width;
	int full_height;
	int corner_x;
	int corner_y;
	int width;
	int height;
	std::vector<float> pixels;  // global buffer

	std::function<void(double progress)> progress_callback;
	std::function<void(Tile &tile)> update_tile_callback;
};