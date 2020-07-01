#pragma once
#include <vector>

class Tile
{
public:
	Tile(unsigned int x, unsigned int y, unsigned int w, unsigned int h) :
		in_offX(x), in_offY(y), in_width(w), in_height(h)
	{
		comps = 3;
		pixels.resize(w * h * 3);
	};
	~Tile()
	{
		pixels.clear();
		pixels.shrink_to_fit();
	};
	
public:
	unsigned int in_offX;
	unsigned int in_offY;
	unsigned int in_width;
	unsigned int in_height;
	unsigned int comps;
	std::vector<float> pixels;
};
