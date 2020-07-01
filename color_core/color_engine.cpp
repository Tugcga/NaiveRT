#include "color_engine.h"
#include <xsi_application.h>

using namespace XSI;

void ColorEngine::set_color(float r, float g, float b, float a)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}

void ColorEngine::set_frame(unsigned int ImageWidth, unsigned int ImageHeight, unsigned int CropLeft, unsigned int CropBottom, unsigned int CropWidth, unsigned int CropHeight)
{
	full_width = ImageWidth;
	full_height = ImageHeight;
	corner_x = CropLeft;
	corner_y = CropBottom;
	width = CropWidth;
	height = CropHeight;

	pixels.resize(width * height * 3);
}

void ColorEngine::render()
{
	unsigned int tileSize = 32;

	for (unsigned y = 0; y <= (height / tileSize); y++)
	{
		for (unsigned x = 0; x <= (width / tileSize); x++)
		{
			unsigned int		ox, oy, sx, sy;

			ox = x * tileSize;
			oy = y * tileSize;
			sx = tileSize;
			sy = tileSize;

			if ((ox + tileSize) > width)
			{
				sx = width - ox;
			}
			else
			{
				sx = tileSize;
			}

			if ((oy + tileSize) > height)
			{
				sy = height - oy;
			}
			else
			{
				sy = tileSize;
			}

			Tile tile(ox + corner_x, oy + corner_y, sx, sy);

			float r = color[0] + (x*y) / 255.0f;
			float g = color[1] + (x*y) / 255.0f;
			float b = color[2] + (x*y) / 255.0f;

			if (update_tile_callback)
			{
				for (unsigned int j = 0; j < sy; j++)
				{
					for (unsigned int i = 0; i < sx; i++)
					{
						tile.pixels[(j * sx + i) * 3] = r;
						tile.pixels[(j * sx + i) * 3 + 1] = g;
						tile.pixels[(j * sx + i) * 3 + 2] = b;

						//set to global buffer
						if ((width * (oy + j) + ox + i) * 3 + 2 > pixels.size())
						{
							XSI::Application().LogMessage(XSI::CString((int)ox) + " " + XSI::CString((int)oy) + " " + XSI::CString((int)i) + " " + XSI::CString((int)j));
						}
						else
						{
							pixels[(width * (oy + j) + ox + i) * 3] = r;
							pixels[(width * (oy + j) + ox + i) * 3 + 1] = g;
							pixels[(width * (oy + j) + ox + i) * 3 + 2] = b;
						}
					}
				}

				update_tile_callback(tile);
			}
		}

		if (progress_callback)
		{
			progress_callback((double)y / double(height / tileSize));
		}
	}
}