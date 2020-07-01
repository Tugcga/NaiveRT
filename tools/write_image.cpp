#include "write_image.h"
#include "logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

float clamp_float(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

void write_ppm(std::string path, int width, int height, std::vector<float> &pixels)
{
	std::ofstream file(path);
	file << "P3\n" << width << ' ' << height << "\n255\n";
	for (int j = height - 1; j >= 0; --j)
	{
		for (int i = 0; i < width; ++i)
		{
			float r = 256 * clamp_float(pixels[(j * width + i) * 3], 0.0, 0.99);
			float g = 256 * clamp_float(pixels[(j * width + i) * 3 + 1], 0.0, 0.99);
			float b = 256 * clamp_float(pixels[(j * width + i) * 3 + 2], 0.0, 0.99);

			file << static_cast<int>(r) << ' '
				<< static_cast<int>(g) << ' '
				<< static_cast<int>(b) << '\n';
		}
	}

	file.close();
}

void write_stb()
{

}

void write(std::string path, int width, int height, int components, std::vector<float> &pixels)
{
	//get the file extension
	size_t point_index = path.find_last_of('.');
	if (point_index != std::string::npos)
	{
		std::string ext = path.substr(point_index + 1, path.length() - point_index - 1);
		if (ext == "ppm")
		{
			write_ppm(path, width, height, pixels);
		}
		else if (ext == "png" || ext == "jpg")
		{
			//prepare the buffer
			stbi_uc* u_pixels = new stbi_uc[pixels.size()];
			for (size_t y = 0; y < height; y++)
			{
				for (size_t x = 0; x < width; x++)
				{//flip by vertical
					for (size_t c = 0; c < components; c++)
					{
						u_pixels[components * ((height - y - 1)*width + x) + c] = int(255.99 * pixels[components * (y*width + x) + c]);
					}
				}
			}
			if (ext == "png")
			{
				stbi_write_png(path.c_str(), width, height, components, u_pixels, width * components);
			}
			else
			{
				stbi_write_jpg(path.c_str(), width, height, components, u_pixels, 100);
			}
			delete[] u_pixels;
		}
		else
		{
			log(CString("Unknown extension ") + CString(ext.c_str()));
		}
	}
}