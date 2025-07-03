#ifndef PIXELFORMAT_H_
#define PIXELFORMAT_H_

#ifndef FC_IMAGE_TYPE_H
#define FC_IMAGE_TYPE_H

namespace FCInterface {

	//todo - fix this - these currently need to match to SG_TEXTURE constants
	enum PixelFormat { PixelFormatNone = 0, PixelFormatGreyscale = 1, PixelFormatRGBA = 2, PixelFormatRGB = 3 };

	unsigned int PixelFormatToBytesPerPixel(PixelFormat format);


}


#endif //!defined FC_IMAGE_TYPE_H


#endif // PIXELFORMAT_H_
