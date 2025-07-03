
#include "ByteVector.h"

#include <cstring>

using namespace FCInterface;
using namespace std;


ByteVector::ByteVector()
{
	bufferSize_ = 0;
	pBuffer_ = 0;
}
/* copy constructor*/
ByteVector::ByteVector(const ByteVector &b) : ByteVector(b.size())
{
	memcpy(pBuffer_, b.buffer(), b.size());
}

ByteVector::ByteVector( ByteVector &&b)
{
	swap(b);
}

ByteVector::ByteVector(unsigned int size)
{
	bufferSize_ = size;
	pBuffer_ = new unsigned char[bufferSize_];
}

ByteVector::~ByteVector()
{
	delete pBuffer_;
}

void ByteVector::clear()
{
	delete pBuffer_;
	bufferSize_ = 0;
	pBuffer_ = 0;
}

void ByteVector::setArray(unsigned char* pBuffer, unsigned int bufferSize, bool deleteExistingBuffer)
{
	if (deleteExistingBuffer) {
		delete pBuffer_;
	}
	pBuffer_ = pBuffer;
	bufferSize_ = bufferSize;
}

void ByteVector::resize(unsigned int width, unsigned int height, unsigned int bytesPerPixel)
{
	bufferSize_ = width * height * bytesPerPixel;

	delete pBuffer_;
	pBuffer_ = new unsigned char[bufferSize_];
}


void ByteVector::resize(unsigned int size)
{
	bufferSize_ = size;

	delete pBuffer_;
	pBuffer_ = new unsigned char[bufferSize_];
}

void ByteVector::swap(ByteVector& v)
{
	unsigned char *pVBuff = v.buffer();
	unsigned int vSize = v.size();
	v.setArray(pBuffer_, bufferSize_, false);
	setArray(pVBuff, vSize, false);
}

void ByteVector::copyIn(unsigned int pos, const unsigned char* pBytes, unsigned int numBytes)
{
	if ((pos + numBytes) > bufferSize_) {
		throw std::out_of_range("ByteVector: index out of bounds");
	}
	else {
		memcpy(pBuffer_ + pos, pBytes, numBytes);
	}
}

