#pragma once
#include "types.h"
#include "gui.h"
#include <strstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>

#define LOOKUP_WHOLE 0
#define LOOKUP_LEFT  1
#define LOOKUP_RIGHT 2

struct WIPGUISpriteClickable;

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

	uint32 lookUp(const float y, const float x, const int32 flag = LOOKUP_WHOLE) const
	{
		// Flip values (array stored differently)
		int32 xIndex;
		int32 yIndex;

		switch (flag)
		{
		case LOOKUP_WHOLE:
		{
			xIndex = (x * width) / scaleW;
			yIndex = (y * height) / scaleH;
		} break;
		case LOOKUP_RIGHT:
		{
			xIndex = (x * width) / scaleW;
			yIndex = ((y * height) / scaleH);
			yIndex *= 0.5f;
			yIndex += height;
		} break;
		case LOOKUP_LEFT:
		{
			xIndex = (x * width) / scaleW;
			yIndex = (y * height) / scaleH;
			yIndex *= 0.5f;
		} break;
		}

		return rgbToHex(
			map[3 * (xIndex * width + yIndex) + 2],
			map[3 * (xIndex * width + yIndex) + 1],
			map[3 * (xIndex * width + yIndex) + 0]);
	}

	// bool: keep -> Keep left if true; Keep right if false
	void crop(const float multiplier, const bool left = true)
	{
		assert(multiplier > 0.0f);
		assert(multiplier < 1.0f);

		int32 newSize = 3 * width * height;
		uint8* newMap = new uint8[newSize];

		int32 newWidth = width * multiplier;
		int32 start = left ? 0 : newWidth;
		int32 end = left ? newWidth : width;

		for (int32 j = 0; j < height; j++)
		{
			for (int32 i = start; i < end; i++)
			{
				newMap[3 * (i + (j * (int32)(width - newWidth))) + 0] = map[3 * (i + (j * width)) + 0];
				newMap[3 * (i + (j * (int32)(width - newWidth))) + 1] = map[3 * (i + (j * width)) + 1];
				newMap[3 * (i + (j * (int32)(width - newWidth))) + 2] = map[3 * (i + (j * width)) + 2];
			}
		}

		memset(map, 0, 3 * width * height);
		delete map;
		map = nullptr;

		scaleH *= 1.0f / multiplier;
		scaleW *= multiplier;
		width = newWidth;
		map = newMap;
	}



	/*
	
	keep all texture but just check for values shifted along

	OR

	keep two textures with same size just shifted left or right

	draw left

	check right for collisions
	
	*/



	void getColourBlocks(std::vector<WIPGUISpriteClickable> vClickable)
	{
		//std::vector<int32> min_ys;

		//bool emptyColumn = true;
		//int32 max_y = 0;
		//int32 min_y = height;
		//for (int32 j = 0; j < height; j++)
		//{
		//	int32 max_x = (width / 2) + 1;
		//	int32 min_x = width;
		//	emptyColumn = true;
		//	for (int32 i = (width / 2) + 1; i < width; i++)
		//	{
		//		uint8 one =   map[3 * (j * width + i) + 0];
		//		uint8 two =   map[3 * (j * width + i) + 1];
		//		uint8 three = map[3 * (j * width + i) + 2];
		//		if (one == 255 && two == 255 && three == 255)
		//		{
		//		}
		//		else
		//		{
		//			emptyColumn = false;
		//			if (i > max_x) max_x = i;
		//			if (i < min_x) min_x = i;
		//			if (j > max_y) max_y = j;
		//			if (j < min_y) min_y = j;
		//			//std::cout << i << "," << j << "\n";
		//		}
		//	}
		//	if (!emptyColumn)
		//	{
		//		std::cout
		//			<< "min_x:" << min_x << " max_x:" << max_x
		//			<< " min_y:" << min_y << " max_y:" << max_y
		//			<< "\n";
		//	}

		//	for (auto y : min_ys)
		//	{
		//		std::cout << y << "\n";
		//	}

		//}
	}
};

struct Pixel24
{
	uint32 colour;
};

struct Pixel32 : Pixel24
{
	uint8 alpha;
};

struct Texture24
{

};

struct Texture32
{

};