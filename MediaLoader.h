#ifndef MEDIALOADER_H_
#define MEDIALOADER_H_


#ifndef MEDIALOADER_H
#define MEDIALOADER_H

#include "gfx/PixelFormat.h"

#include "gfx/ByteVector.h"
#include "FileInfo.h"
#include <string>

namespace FCInterface {

	class MediaLoader {
	public:

		static FileMediaType guessMediaType(const std::string& filename);

		static bool loadImageFromMemory(const ByteVector& fileContents, FileMediaType mediaType, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0);

		static bool loadJPEGThumbFromMemory(const ByteVector& fileContents, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0);


		static bool loadJPEGFromMemory(const ByteVector& fileContents, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0);

		static bool loadImage(const std::string& filename, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0, unsigned int maxFileSize = 0);
		static bool loadPNGFromMemory(const ByteVector& fileContents, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0);

		static bool loadPNG(const std::string& filename, unsigned int& width, unsigned int& height, ByteVector& img, FCInterface::PixelFormat& imageTypeOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0, unsigned int maxFileSize = 0);
		static bool loadPNG(const std::string& filename, unsigned int& widthOut, unsigned int& heightOut,
			unsigned int& bppOut, ByteVector& buffer, bool makePOT, float& usageOutX, float& usageOutY, FCInterface::PixelFormat& imageTypeOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0, unsigned int maxFileSize = 0);

		static bool loadPNGFromMemory(const ByteVector& fileContents, unsigned int& widthOut, unsigned int& heightOut,
			unsigned int& bppOut, ByteVector& buffer, bool makePOT, float& usageOutX, float& usageOutY, FCInterface::PixelFormat& imageTypeOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0);


		static bool loadJPEG(const std::string& filename, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth = 0, unsigned int maxHeight = 0, unsigned int maxFileSize = 0);


		static bool savePNG(const std::string& filename, ByteVector& pixels, unsigned int width, unsigned int height, PixelFormat pixelFormat, bool flipVertical = false);

		static void saveFImage(const std::string& filename, const ByteVector& pixels, unsigned int width, unsigned int height, PixelFormat pixelFormat);

		static bool loadFImage(const std::string& filename, ByteVector& imgOut, unsigned int& widthOut, unsigned int& heightOut, PixelFormat& pixelFormatOut);

		static bool loadFImage(const std::string& filename, ByteVector& imgOut, unsigned int& widthOut, unsigned int& heightOut, PixelFormat& pixelFormatOut, bool makePOT, float& usageXOut, float& usageYOut);


	};
}

#endif //!defined MEDIALOADER_H


#endif // MEDIALOADER_H_
