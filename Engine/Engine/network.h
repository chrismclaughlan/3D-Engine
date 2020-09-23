#pragma once
#include "types.h"
#include <memory>
#include <assert.h>
#include <string>

namespace NETWORK
{
	static constexpr const int maxBufferSize = 100;

	struct Packet
	{
		int numBytes = 0;
		int8 buffer[maxBufferSize];

		Packet() : numBytes(0)
		{
			memset(buffer, 0, maxBufferSize);
			assert(numBytes < maxBufferSize);
		}

		Packet(const std::string _buffer)
		{
			if (_buffer.length() >= maxBufferSize)
				numBytes = maxBufferSize - 1;
			else
				numBytes = _buffer.length();
			_buffer.copy(buffer, numBytes);
			assert(numBytes < maxBufferSize);
		}

		Packet(const int8* _buffer)
		{
			strncpy_s(buffer, _buffer, _TRUNCATE);
			numBytes = std::strlen(buffer);
			assert(numBytes < maxBufferSize);
		}

		Packet(const int8* _buffer, const int _numBytes)
			: numBytes(_numBytes)
		{
			//strncpy_s(buffer, _numBytes, _buffer, _TRUNCATE);
			strncpy_s(buffer, _buffer, _TRUNCATE);
			assert(numBytes < maxBufferSize);
		}

		Packet& operator=(const Packet& a)
		{
			numBytes = a.numBytes;
			//strncpy_s(buffer, a.numBytes, a.buffer, _TRUNCATE);
			strncpy_s(buffer, a.buffer, _TRUNCATE);
			assert(numBytes < maxBufferSize);
			return *this;
		}

		bool operator==(const Packet& a) const
		{
			return (strcmp(buffer, a.buffer) == 0) ? true : false;
		}

		bool contains(const char* a)
		{
			return (strcmp(buffer, a) == 0) ? true : false;
		}
	};
}  // namespace NETWORK