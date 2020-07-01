#pragma once
#include <functional>
#include <vector>

#include "../nrt_core/tile.h"
#include "../renderer/render_buffer.h"

class ColorEngine
{
public:
	ColorEngine() { pixels.resize(0); };
	~ColorEngine() 
	{
		pixels.clear();
		pixels.shrink_to_fit();
	};
	void set_frame(unsigned int ImageWidth, unsigned int ImageHeight, unsigned int CropLeft, unsigned int CropBottom, unsigned int CropWidth, unsigned int CropHeight);
	void set_color(float r, float g, float b, float a);
	void set_progress_callback(std::function<void(double progress)> callback) { progress_callback = callback; }
	void set_update_tile_callback(std::function<void(Tile &tile)> callback) { update_tile_callback = callback; }
	void render();
	std::vector<float> &get_buffer_pixels() { return pixels; };

private:
	unsigned int full_width, full_height;  // full frame size
	unsigned int corner_x, corner_y;  // coordinates of the left bottom corner
	unsigned int width, height;  // actual render size
	std::vector<float> pixels;

	float color[4];
	std::function<void(double progress)> progress_callback;
	std::function<void(Tile &tile)> update_tile_callback;
};