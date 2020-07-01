#pragma once
#include <xsi_renderercontext.h>

using namespace XSI;

class FragmentUInt : public RendererImageFragment
{
public:
	FragmentUInt(
		unsigned int in_offX, unsigned int in_offY, unsigned int in_width, unsigned int in_height, double in_color[4])
	{
		offX = in_offX;
		offY = in_offY;
		width = in_width;
		height = in_height;

		unsigned int		r, g, b, a;

		r = (unsigned int)(in_color[0] * 255.0);
		g = (unsigned int)(in_color[1] * 255.0);
		b = (unsigned int)(in_color[2] * 255.0);
		a = (unsigned int)(in_color[3] * 255.0);

		color = (a << 24) | (b << 16) | (g << 8) | (r);
	}

	unsigned int GetOffsetX() const { return(offX); }
	unsigned int GetOffsetY() const { return(offY); }
	unsigned int GetWidth() const { return(width); }
	unsigned int GetHeight() const { return(height); }
	bool GetScanlineRGBA(unsigned int in_uiRow, siImageBitDepth in_eBitDepth, unsigned char *out_pScanline) const
	{
		unsigned int		*pScanline = (unsigned int *)out_pScanline;
		for (unsigned int i = 0; i < width; i++)
		{
			pScanline[i] = color;
		}

		return true;
	}

private:
	unsigned int	offX, offY, width, height;
	unsigned int	color;
};

struct RGBA
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

inline unsigned char linearToSRGB(float v)
{
	if (v <= 0.0f)
		return 0;
	if (v >= 1.0f)
		return 255;
	if (v <= 0.0031308f)
		return  (unsigned char)((12.92f * v * 255.0f) + 0.5f);
	return (unsigned char)(((1.055f * pow(v, 1.0f / 2.4f)) - 0.055f) * 255.0f + 0.5f);
}

inline unsigned char linearClamp(float v)
{
	if (v <= 0.0f)
		return 0;
	if (v >= 1.0f)
		return 255;
	return (unsigned char)(v * 255.0);
}

class FragmentFloat : public RendererImageFragment
{
public:
	FragmentFloat(unsigned int in_offX, unsigned int in_offY, unsigned int in_width, unsigned int in_height, std::vector<float> &_pixels, int _components, bool _apply_srgb)
	{
		offX = in_offX;
		offY = in_offY;
		width = in_width;
		height = in_height;
		components = _components;
		apply_srgb = _apply_srgb;
		fPixels = _pixels;
	}

	unsigned int GetOffsetX() const { return(offX); }
	unsigned int GetOffsetY() const { return(offY); }
	unsigned int GetWidth() const { return(width); }
	unsigned int GetHeight() const { return(height); }

	bool GetScanlineRGBA(unsigned int in_uiRow, XSI::siImageBitDepth in_eBitDepth, unsigned char *out_pScanline) const
	{
		RGBA* pScanline = (RGBA*)out_pScanline;
		for (unsigned int i = 0; i < width; i++)
		{
			unsigned int indexShift = in_uiRow * width;
			pScanline[i].a = components == 4 ? (unsigned char)(fPixels[4 * (indexShift + i) + 3] * 255.0) : (unsigned char)255.0;
			if (apply_srgb)
			{
				if (components >= 3)
				{
					pScanline[i].r = linearToSRGB(fPixels[components * (indexShift + i)]);
					pScanline[i].g = linearToSRGB(fPixels[components * (indexShift + i) + 1]);
					pScanline[i].b = linearToSRGB(fPixels[components * (indexShift + i) + 2]);
				}
				else
				{
					unsigned char v = linearToSRGB(fPixels[indexShift + i]);
					pScanline[i].r = v;
					pScanline[i].g = v;
					pScanline[i].b = v;
				}
			}
			else
			{
				if (components >= 3)
				{
					pScanline[i].r = linearClamp(fPixels[components * (indexShift + i)]);
					pScanline[i].g = linearClamp(fPixels[components * (indexShift + i) + 1]);
					pScanline[i].b = linearClamp(fPixels[components * (indexShift + i) + 2]);
				}
				else
				{
					unsigned char v = linearClamp(fPixels[indexShift + i]);
					pScanline[i].r = v;
					pScanline[i].g = v;
					pScanline[i].b = v;
				}
			}
		}

		return true;
	}

private:
	unsigned int offX, offY, width, height;
	int components;
	bool apply_srgb;
	std::vector<float> fPixels;
};