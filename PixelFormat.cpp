
#include "PixelFormat.h"




unsigned int FCInterface::PixelFormatToBytesPerPixel(FCInterface::PixelFormat format)
{
	switch (format) {
	case PixelFormatGreyscale:
		return 1;
	case PixelFormatRGB:
		return 3;
	case PixelFormatRGBA:
		return 4;
	default:
		return 0;
	}
}