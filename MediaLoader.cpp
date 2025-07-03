
#include "MediaLoader.h"

#include "Log.h"
#include "gfx/lodepng.h"
#include "gfx/ujpeg.h"
#include "sgUtil.h"
#include "FileInfo.h"

#include <fstream>
#include <vector>

using namespace FCInterface;
using namespace std;

bool MediaLoader::loadPNG(const std::string& filename, unsigned int& width, unsigned int& height, ByteVector& img, FCInterface::PixelFormat& imageTypeOut, unsigned int maxWidth, unsigned int maxHeight, unsigned int maxFileSize)
{
	unsigned int bppOut;
	float usageX, usageY;

	return loadPNG(filename, width, height, bppOut, img, false, usageX, usageY, imageTypeOut, maxWidth, maxHeight, maxFileSize);
}


bool MediaLoader::loadPNG(const std::string& filename, unsigned int& widthOut, unsigned int& heightOut,
	unsigned int& bppOut, ByteVector& buffer, bool makePOT, float& usageOutX, float& usageOutY, FCInterface::PixelFormat& imageTypeOut, unsigned int maxWidth, unsigned int maxHeight, unsigned int maxFileSize)
{
	ByteVector fileContents;
	std::ifstream file(filename, ios_base::binary);
	if (!file.is_open()) {
		Log(LOG_ERROR, "Media Loader: Could not open PNG file %s", filename.c_str());
		return false;
	}

	file.seekg(0, std::ios_base::end);
	std::streampos fileSize = file.tellg();
	if (maxFileSize > 0 && fileSize > maxFileSize) {
		Log(LOG_ERROR, "Media Loader: File %s (size %u bytes) exceeded maximum size of %u bytes", filename.c_str(), fileSize, maxFileSize);
		return false;
	}
	fileContents.resize((unsigned int)fileSize);
	file.seekg(0, std::ios_base::beg);

	file.read((char*)&fileContents[0], fileSize);
	file.close();

	if (fileContents.size() == 0) {
		Log(LOG_ERROR, "MediaLoader - Could not load PNG file: %s", filename.c_str());
		return false;
	}


	if (!loadPNGFromMemory(fileContents, widthOut, heightOut, bppOut, buffer, makePOT, usageOutX, usageOutY, imageTypeOut, maxWidth, maxHeight))
	{
		Log(LOG_ERROR, "Could not parse PNG %s", filename.c_str());
		return false;
	}

	return true;

}

bool MediaLoader::loadPNGFromMemory(const ByteVector& fileContents, unsigned int& widthOut, unsigned int& heightOut, 
	ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, 
	unsigned int maxWidth, unsigned int maxHeight)
{
	unsigned int bppOut;
	float usageX, usageY;
	return loadPNGFromMemory(fileContents, widthOut, heightOut, bppOut, imgOut, false, usageX, usageY, pixelFormatOut, maxWidth, maxHeight);

}

bool MediaLoader::loadPNGFromMemory(const ByteVector& fileContents, unsigned int& widthOut, unsigned int& heightOut,
	unsigned int& bppOut, ByteVector& buffer, bool makePOT, float& usageOutX, float& usageOutY, FCInterface::PixelFormat& imageTypeOut, unsigned int maxWidth, unsigned int maxHeight)
{
	//unsigned int error = lodepng::load_file(fileContents, filename); //load the image file with given filename

	lodepng::State state;
	state.maxImageWidth = maxWidth;
	state.maxImageHeight = maxHeight;
	state.decoder.color_convert = false;

	if (makePOT) {

		ByteVector image;
		unsigned int width, height;
		unsigned error = lodepng::decode(image, width, height, state, fileContents);

		if (error)
		{

			return false;
		}

		if (state.info_raw.bitdepth != 8 || !(state.info_raw.colortype == LCT_RGBA || state.info_raw.colortype == LCT_GREY)) {
			Log(LOG_ERROR, "Loaded PNG but format is unsupported");
			return false;
		}

		widthOut = 1;
		heightOut = 1;
		while (widthOut < width)
			widthOut <<= 1;
		while (heightOut < height)
			heightOut <<= 1;

		usageOutX = (float)width / (float)widthOut;
		usageOutY = (float)height / (float)heightOut;

		if (state.info_raw.colortype == LCT_RGBA) {
			bppOut = 32;
			imageTypeOut = PixelFormat::PixelFormatRGBA;
		} 
		else {
			bppOut = 8;
			imageTypeOut = PixelFormat::PixelFormatGreyscale;
		}
		

		if (bppOut == 32)
		{
			buffer.resize(4 * widthOut * heightOut);
			unsigned int* pixIn = (unsigned int*)&image[0];
			unsigned int* pixOut = (unsigned int*)&buffer[0];
			
			for (unsigned int y = 0; y < height; y++) {
				for (unsigned int x = 0; x < width; x++) {
					pixOut[y*widthOut + x] = pixIn[y*width + x];
				}
			}
		}
		else if (bppOut == 8)
		{
			buffer.resize(widthOut * heightOut);
			unsigned char* pixIn = (unsigned char*)&image[0];
			unsigned char* pixOut = (unsigned char*)&buffer[0];

			for (unsigned int y = 0; y < height; y++) {
				for (unsigned int x = 0; x < width; x++) {
					pixOut[y*widthOut + x] = pixIn[y*width + x];
				}
			}
		}
	} else {
		
		unsigned error = lodepng::decode(buffer, widthOut, heightOut, state, fileContents);

		if (error)
		{
			Log(LOG_ERROR, "Could not load PNG %s", lodepng_error_text(error));
			return false;
		}



		if (state.info_raw.bitdepth != 8 || !(state.info_raw.colortype == LCT_RGBA || state.info_raw.colortype == LCT_GREY || state.info_raw.colortype == LCT_RGB || state.info_raw.colortype == LCT_PALETTE)) {
			Log(LOG_ERROR, "Loaded PNG  but format is unsupported");
			return false;
		}

		usageOutX = 1;
		usageOutY = 1;

		if (state.info_raw.colortype == LCT_RGBA) {
			bppOut = 32;
			imageTypeOut = PixelFormat::PixelFormatRGBA;
		} 
		else if (state.info_raw.colortype == LCT_RGB) {
			bppOut = 24;
			imageTypeOut = PixelFormat::PixelFormatRGB;
		}
		else if (state.info_raw.colortype == LCT_PALETTE) {
			bppOut = 8;
			imageTypeOut = PixelFormat::PixelFormatGreyscale;
		}
		else {
			bppOut = 8;
			imageTypeOut = PixelFormat::PixelFormatGreyscale;
		}

	}

	return true;
}


bool MediaLoader::loadJPEGThumbFromMemory(const ByteVector& fileContents, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth, unsigned int maxHeight)
{
	uJPEG jpeg;
	jpeg.setMaxDimensions(maxWidth, maxHeight);
	jpeg.setThumbnailMode(true);

	if (!jpeg.decode(&fileContents[0], (int)fileContents.size())) {
		Log(LOG_ERROR, "Media Loader: Error decoding the input file %u", ujGetError());
		return false;
	}

	ByteVector thumbData;
	if (jpeg.getThumb(thumbData)) {
		loadJPEGFromMemory(thumbData, widthOut, heightOut, imgOut, pixelFormatOut, maxWidth, maxHeight);
		return true;
	}
	else {
		return false;
	}

}

bool MediaLoader::loadJPEG(const std::string& filename, unsigned int& widthOut, unsigned int& heightOut, 
	ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth, unsigned int maxHeight, unsigned int maxFileSize)
{
	std::ifstream file(filename, ios_base::binary);
	if (!file.is_open()) {
		Log(LOG_ERROR, "Media Loader: Could not open file %s", filename.c_str());
		return false;
	}

	ByteVector jpegData;

	file.seekg(0, std::ios_base::end);
	std::streampos fileSize = file.tellg();
	if (maxFileSize == 0 || maxFileSize >= fileSize) {
		jpegData.resize((unsigned int)fileSize);
		file.seekg(0, std::ios_base::beg);
		file.read((char*)&jpegData[0], fileSize);
		file.close();

		return loadJPEGFromMemory(jpegData, widthOut, heightOut, imgOut, pixelFormatOut, maxWidth, maxHeight);
	}
	else {
		Log(LOG_ERROR, "Media Loader: Ignoring JPEG %s - file is too large", filename.c_str());
		return false;
	}

}




bool MediaLoader::loadJPEGFromMemory(const ByteVector& fileContents, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth, unsigned int maxHeight)
{
	uJPEG jpeg;
	//	jpeg.setChromaMode(UJ_CHROMA_MODE_FAST);

	jpeg.setMaxDimensions(maxWidth, maxHeight);

	if (!jpeg.decode(&fileContents[0], (int)fileContents.size())) {
		Log(LOG_ERROR, "Media Loader: Error decoding the input file %u", ujGetError());
		return false;
	}


	bool result = true;
	unsigned int imageSize = jpeg.getImageSize();
	unsigned int indexIn = 0, indexOut = 0;
	ByteVector pixels;
	jpeg.getImage(pixels);
	unsigned char* pPixelsIn = &pixels[0];

	if (jpeg.isColor() == 1 && imageSize == (jpeg.getWidth() * jpeg.getHeight() * 4)) {
		pixelFormatOut = PixelFormat::PixelFormatRGBA;
	}
	else if (imageSize == (jpeg.getWidth() * jpeg.getHeight()) * 3) {
		pixelFormatOut = PixelFormat::PixelFormatRGB;
	}
	else if (imageSize == (jpeg.getWidth() * jpeg.getHeight())) {
		pixelFormatOut = PixelFormat::PixelFormatGreyscale;
	}
	else {
		result = false;
	}

	if (result) {
		imgOut.resize(pixels.size());
		imgOut.swap(pixels);

		widthOut = (unsigned int)jpeg.getWidth();
		heightOut = (unsigned int)jpeg.getHeight();
	}


	return result;




}

bool MediaLoader::savePNG(const std::string& filename, ByteVector& pixels, unsigned int width, unsigned int height, PixelFormat pixelFormat, bool flipVertical)
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


bool MediaLoader::loadImage(const std::string& filename, unsigned int& widthOut, unsigned int& heightOut,
	ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth,
	unsigned int maxHeight, unsigned int maxFileSize)
{
	bool r;

	if (Util::checkExtension(filename, "jpeg", "jpg", 0)) {
		r = MediaLoader::loadJPEG(filename, widthOut, heightOut, imgOut, pixelFormatOut, maxWidth, maxHeight, maxFileSize);
	}
	else if (Util::checkExtension(filename, "png", 0)) {
		r = MediaLoader::loadPNG(filename, widthOut, heightOut, imgOut, pixelFormatOut, maxWidth, maxHeight, maxFileSize);
	}
	else {
		r = false;
	}

	return r;
}

FileMediaType MediaLoader::guessMediaType(const std::string& filename)
{
	if (Util::checkExtension(filename, "jpeg", "jpg", 0)) {
		return FileMediaType::FMT_JPEG;
	}
	else if (Util::checkExtension(filename, "png", 0)) {
		return FileMediaType::FMT_PNG;
	}
	else if (Util::checkExtension(filename, "fim", 0)) {
		return FileMediaType::FMT_FIMAGE;
	}
	else {
		return FileMediaType::FMT_UNKNOWN;
	}
}
bool MediaLoader::loadImageFromMemory(const ByteVector& fileContents, FileMediaType mediaType, unsigned int& widthOut, unsigned int& heightOut, ByteVector& imgOut, FCInterface::PixelFormat& pixelFormatOut, unsigned int maxWidth, unsigned int maxHeight)
{
	if (mediaType == FileMediaType::FMT_JPEG) {
		return loadJPEGFromMemory(fileContents, widthOut, heightOut, imgOut, pixelFormatOut, maxWidth, maxHeight);
	} else if (mediaType == FileMediaType::FMT_PNG) {
		return loadPNGFromMemory(fileContents, widthOut, heightOut, imgOut, pixelFormatOut, maxWidth, maxHeight);
	}
	else {
		return false;
	}

}


void MediaLoader::saveFImage(const std::string& filename, const ByteVector& pixels, unsigned int width, unsigned int height, PixelFormat pixelFormat)
{
	ofstream fh(filename, ios_base::binary | ios_base::trunc | ios_base::out);

	unsigned int special = 0xAD;
	unsigned int version = 0x01;
	unsigned int bytesPerPixel = PixelFormatToBytesPerPixel(pixelFormat);

	fh.write((char*)&special, 4);
	fh.write((char*)&version, 4);
	fh.write((char*)&bytesPerPixel, 4);
	fh.write((char*)&width, 4);
	fh.write((char*)&height, 4);

	fh.write((char*)pixels.buffer(), pixels.size());


}

bool MediaLoader::loadFImage(const std::string& filename, ByteVector& imgOut, unsigned int& widthOut, unsigned int& heightOut, PixelFormat& pixelFormatOut)
{
	float xUsage;
	float yUsage;
	return loadFImage(filename, imgOut, widthOut, heightOut, pixelFormatOut, false, xUsage, yUsage);
}

bool MediaLoader::loadFImage(const std::string& filename, ByteVector& imgOut, unsigned int& widthOut, unsigned int& heightOut, PixelFormat& pixelFormatOut, bool makePOT, float& usageXOut, float& usageYOut)
{
	ifstream fh(filename, ios_base::binary | ios_base::in);

	if (!fh.is_open()) {
		Log(LOG_ERROR, "Could not open FImage file %s", filename.c_str());
		return false;
	}
	fh.seekg(0, std::ios_base::end);
	unsigned int fileSize = (unsigned int)fh.tellg();
	fh.seekg(0, std::ios_base::beg);

	if (fileSize < 21) { //minimum valid file size
		Log(LOG_ERROR, "FImage file %s appears truncated - cannot load", filename.c_str());
		return false;
	}


	unsigned int special, version, bytesPerPixel, imgWidth, imgHeight;

	fh.read((char*)&special, 4);
	fh.read((char*)&version, 4);
	fh.read((char*)&bytesPerPixel, 4);
	fh.read((char*)&imgWidth, 4);
	fh.read((char*)&imgHeight, 4);

	if (special != 0xAD) {
		Log(LOG_ERROR, "FImage file %s has incorrect magic code at file start - cannot load", filename.c_str());
		return false;
	}

	if (version != 0x01) {
		Log(LOG_ERROR, "FImage file %s has unknown version - cannot load", filename.c_str());
		return false;
	}

	if (imgWidth > 4096 || imgHeight > 4096) { //hard coded limits
		Log(LOG_ERROR, "FImage file %s: dimensions are too great - cannot load", filename.c_str());
		return false;
	}

	unsigned int pixelDataLength = fileSize - 20;
	unsigned int expectedPixels = imgWidth * imgHeight;
	if (bytesPerPixel == 0x01) {
		pixelFormatOut = PixelFormat::PixelFormatGreyscale;
	}
	else if (bytesPerPixel == 0x03) {
		pixelFormatOut = PixelFormat::PixelFormatRGB;
		expectedPixels *= 3;
	} 
	else if (bytesPerPixel == 0x04) {
		pixelFormatOut = PixelFormat::PixelFormatRGBA;
		expectedPixels <<= 2;
	}
	else {
		Log(LOG_ERROR, "FImage file %s has unknown pixel format - cannot load", filename.c_str());
		return false;
	}

	if (pixelDataLength != expectedPixels) {
		Log(LOG_ERROR, "FImage file %s has incorrect payload length - cannot load", filename.c_str());
		return false;
	}


	if (makePOT) {
		widthOut = 1;
		heightOut = 1;
		while (widthOut < imgWidth)
			widthOut <<= 1;
		while (heightOut < imgHeight)
			heightOut <<= 1;
	}
	else {
		widthOut = imgWidth;
		heightOut = imgHeight;
	}

	imgOut.resize(widthOut * heightOut * bytesPerPixel);

	char* pBufferOut = (char*)imgOut.buffer();
	if (widthOut > imgWidth) {
		//copy line by line 
		unsigned int imgBytesPerLine = imgWidth * bytesPerPixel;
		unsigned int strideOut = widthOut * bytesPerPixel;
		for (unsigned int y = 0; y < imgHeight; y++) {
			fh.read(pBufferOut, imgBytesPerLine);
			pBufferOut += strideOut;
		}
	}
	else {
		fh.read(pBufferOut, imgOut.size());
	}
	
	usageXOut = (float)imgWidth / (float)widthOut;
	usageYOut = (float)imgHeight / (float)heightOut;





	return true;
}