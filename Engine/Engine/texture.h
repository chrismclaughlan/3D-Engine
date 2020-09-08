#pragma once
#include "types.h"
#include <strstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>

class Texture
{
public:
	int32 width = -1;
	int32 height = -1;
	float scaleW = 1.0f;
	float scaleH = 1.0f;
	uint8* map = nullptr;

public:
	~Texture()
	{
		if (map != nullptr)
		{
			delete[] map;
			map = nullptr;
		}
	}

	//inline void writeTo(int x, int y, uint32 a)
	//{
	//	map[x * width + y] = a;
	//}
	//
	//inline uint32 readFrom(int x, int y) const
	//{
	//	return map[x * width + y];
	//}

	//static long int binToHex(long int binaryval)
	//{
	//	long int hexadecimalval = 0, i = 1, remainder;
	//
	//	while (binaryval != 0)
	//	{
	//		remainder = binaryval % 10;
	//		hexadecimalval = hexadecimalval + remainder * i;
	//		i = i * 2;
	//		binaryval = binaryval / 10;
	//	}
	//
	//	return hexadecimalval;
	//}

	static uint32 rgbToHex(const uint8 r, const uint8 g, const uint8 b)
	{
		uint32 h = 0x0;
		h += r << 16;
		h += g << 8;
		h += b << 0;
		return h;
	}

	bool LoadTextureFromBMP(const char* filename)
	{
		int i;
		FILE* f;
		errno_t err = fopen_s(&f, filename, "r");
		if (err != 0)
		{
			std::cerr << "Error opening file " << filename << "\n";
			return false;
		}
		unsigned char info[54];

		// read the 54-byte header
		fread(info, sizeof(uint8), 54, f);

		// extract image height and width from header
		width = *(int*)& info[18];
		height = *(int*)& info[22];
		if (width < height)
		{
			scaleH = height / width;
			scaleW = 1.0f / scaleH;
		}
		else
		{
			scaleW = width / height;
			scaleH = 1.0f / scaleW;
		}

		// allocate 3 bytes per pixel
		int size = 3 * width * height;
		map = new uint8[size];

		// read the rest of the data at once
		fread(map, sizeof(uint8), size, f);
		fclose(f);

		return true;
	}

	uint32 lookUp(float x, float y) const
	{
		int32 xIndex = (x * width) / scaleW;
		int32 yIndex = (y * height) / scaleH;

		return rgbToHex(
			map[3 * (xIndex * width + yIndex) + 2],
			map[3 * (xIndex * width + yIndex) + 1],
			map[3 * (xIndex * width + yIndex) + 0]);
	}
};