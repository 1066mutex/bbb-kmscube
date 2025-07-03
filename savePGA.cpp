
#include "lodepng.h"
#include "Log.h" //TODO debug



/*The PNG color types (also used for raw).*/
typedef enum LodePNGColorType
{
  LCT_GREY = 0, /*greyscale: 1,2,4,8,16 bit*/
  LCT_RGB = 2, /*RGB: 8,16 bit*/
  LCT_PALETTE = 3, /*palette: 1,2,4,8 bit*/
  LCT_GREY_ALPHA = 4, /*greyscale with alpha: 8,16 bit*/
  LCT_RGBA = 6 /*RGB with alpha: 8,16 bit*/
} LodePNGColorType;

unsigned lodepng_encode_file(
                const char* filename, // file to save to.
                const unsigned char* image,
                unsigned w,
                unsigned h,
                LodePNGColorType colortype,
                 unsigned bitdepth
                )
{
  unsigned char* buffer;
  size_t buffersize;
  unsigned error = lodepng_encode_memory(&buffer, &buffersize, image, w, h, colortype, bitdepth);
  if(!error) error = lodepng_save_file(buffer, buffersize, filename);
  lodepng_free(buffer);
  return error;
}

unsigned lodepng_encode32_file(const char* filename, const unsigned char* image, unsigned w, unsigned h)
{
  return lodepng_encode_file(filename, image, w, h, LCT_RGBA, 8);
}

unsigned lodepng_encode24_file(const char* filename, const unsigned char* image, unsigned w, unsigned h)
{
  return lodepng_encode_file(filename, image, w, h, LCT_RGB, 8);
}



bool savePNG(const std::string& filename, ByteVector& pixels, unsigned int width, unsigned int height, PixelFormat pixelFormat, bool flipVertical)
{
	unsigned int bytesPerPixel = PixelFormatToBytesPerPixel(pixelFormat);
	if (pixels.size() != (width * height * bytesPerPixel)) {
		Log(LOG_ERROR, "Invalid data sent to savePNG32. #pixels must equal width * height * 4");
		return false;
	}

	if (flipVertical) {
		ByteVector flipped(pixels.size());

		if (pixelFormat == PixelFormat::PixelFormatRGBA) {

			unsigned int* pSrc = (unsigned int*)&pixels[0];
			unsigned int* pDest = (unsigned int*)&flipped[0];

			for (unsigned int y = 0; y < height; y++) {
				for (unsigned int x = 0; x < width; x++) {
					pDest[y*width + x] = pSrc[(height - y - 1) * width + x];
				}
			}

		}
		else {
			unsigned char* pSrc = (unsigned char*)&pixels[0];
			unsigned char* pDest = (unsigned char*)&flipped[0];
			unsigned int bytesPerLine = bytesPerPixel * width;
			for (unsigned int y = 0; y < height; y++) {
				for (unsigned int x = 0; x < bytesPerLine; x++) {
					pDest[y*bytesPerLine + x] = pSrc[(height - y - 1) * bytesPerLine + x];
				}
			}
		}
		pixels.swap(flipped);
		
	}

	if (pixelFormat == PixelFormat::PixelFormatRGBA) {

		if (lodepng_encode32_file(filename.c_str(), &pixels[0], width, height) != 0) {
			Log(LOG_ERROR, "Could not save 32-bit PNG at %s", filename.c_str());
			return false;
		}
	}
	else if (pixelFormat == PixelFormat::PixelFormatRGB) {
		if (lodepng_encode24_file(filename.c_str(), &pixels[0], width, height) != 0) {
			Log(LOG_ERROR, "Could not save 24-bit PNG at %s", filename.c_str());
			return false;
		}
	}
	else {
		Log(LOG_ERROR, "Could not save 8-bit PNG at %s", filename.c_str());
		return false;
	}

	return true;
}