#pragma once

#include <vector>
#include <xsi_application.h>

class RenderBuffer
{
public:
	RenderBuffer() { pixels.resize(0); };
	RenderBuffer(ULONG ImageWidth, ULONG ImageHeight, ULONG CropLeft, ULONG CropBottom, ULONG CropWidth, ULONG CropHeight, ULONG coms) :
		full_width(ImageWidth),
		full_height(ImageHeight),
		corner_x(CropLeft),
		corner_y(CropBottom),
		width(CropWidth),
		height(CropHeight),
		components(coms)
	{
		pixels.resize(width * height * components);
	};
	~RenderBuffer()
	{
		pixels.clear();
		pixels.shrink_to_fit();
	};

	void clean()
	{
		pixels.clear();
		pixels.shrink_to_fit();
	};

public:
	unsigned int full_width, full_height;  // full frame size
	unsigned int corner_x, corner_y;  // coordinates of the left bottom corner
	unsigned int width, height;  // actual render size
	ULONG components;

	std::vector<float> pixels;
};