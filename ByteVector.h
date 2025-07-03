#ifndef BYTEVECTOR_H_
#define BYTEVECTOR_H_
#ifndef FCUI_PIXEL_VECTOR_H
#define FCUI_PIXEL_VECTOR_H


#include <stdexcept>
#include <vector>

namespace FCInterface {
	class ByteVector {
	public:
		ByteVector();
		ByteVector(const ByteVector &b);
		ByteVector(ByteVector &&b);
		ByteVector(unsigned int size); 
		~ByteVector();
		void setArray(unsigned char* pBuffer, unsigned int bufferSize, bool deleteExistingBuffer = true);
		void clear();
		bool empty() const { return bufferSize_ == 0; }
		void resize(unsigned int width, unsigned int height, unsigned int bytesPerPixel);
		void resize(unsigned int size);
		unsigned char* buffer() const { return pBuffer_; }
		void swap(ByteVector& v);
		unsigned int size() const { return bufferSize_; }

		void copyIn(unsigned int pos, const unsigned char* pBytes, unsigned int numBytes);

		unsigned char& operator[](const unsigned int pos) const {
			if (pos >= bufferSize_)
				throw std::out_of_range("ByteVector: index out of bounds");
			return (this->pBuffer_[pos]);
		}
	private:
		unsigned int bufferSize_;
		unsigned char* pBuffer_;
	};


}

#endif //!defined FCUI_PIXEL_VECTOR_H


#endif // BYTEVECTOR_H_
