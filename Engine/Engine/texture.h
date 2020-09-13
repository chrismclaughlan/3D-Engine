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

struct GUISpriteClickable;

class Texture
{
protected:
	int32 width = -1;
	int32 height = -1;
	float scaleW = 1.0f;
	float scaleH = 1.0f;

public:
	virtual const int32 headerSize() const = 0;
	virtual void writeData(FILE* f) = 0;
	virtual uint32 lookUp(const float y, const float x, const int32 segments = 1, const int32 index = 1) const = 0;
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

		uint8* info = new uint8[headerSize()];

		// read the 54-byte header
		fread(info, sizeof(uint8), headerSize(), f);

		const int32 bOffset = info[10];

		std::cout << "Offset: " << bOffset << std::endl;

		// extract image height and width from header
		width = *(int32*)& info[18];
		height = *(int32*)& info[22];
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

		writeData(f);

		fclose(f);

		delete[] info;
		info = nullptr;

		return true;
	}

};

class Texture24 : public Texture
{
private:
	uint8* data = nullptr;

public:
	~Texture24()
	{
		if (data != nullptr)
		{
			delete[] data;
			data = nullptr;
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

	const int32 headerSize() const { return 54; };
	static uint32 rgbToHex(const uint8 r, const uint8 g, const uint8 b)
	{
		uint32 h = 0x0;
		h += r << 16;
		h += g << 8;
		h += b << 0;
		return h;
	}

	void writeData(FILE* f) override
	{
		// allocate 3 bytes per pixel
		int size = 3 * width * height;
		data = new uint8[size];

		// read the rest of the data at once
		fread(data, sizeof(uint8), size, f);
	}

	uint32 lookUp(const float y, const float x, const int32 segments = 1, const int32 index = 1) const override
	{
		// Flip values (array stored differently)
		int32 xIndex;
		int32 yIndex;

		xIndex = (x * width) / scaleW;
		yIndex = (y * height) / scaleH;

		//switch (flag)
		//{
		//case LOOKUP_WHOLE:
		//{
		//	xIndex = (x * width) / scaleW;
		//	yIndex = (y * height) / scaleH;
		//} break;
		//case LOOKUP_RIGHT:
		//{
		//	xIndex = (x * width) / scaleW;
		//	yIndex = ((y * height) / scaleH);
		//	yIndex *= 0.5f;
		//	yIndex += height;
		//} break;
		//case LOOKUP_LEFT:
		//{
		//	xIndex = (x * width) / scaleW;
		//	yIndex = (y * height) / scaleH;
		//	yIndex *= 0.5f;
		//} break;
		//}

		return rgbToHex(
			data[3 * (xIndex * width + yIndex) + 2],
			data[3 * (xIndex * width + yIndex) + 1],
			data[3 * (xIndex * width + yIndex) + 0]);
	}

	// bool: keep -> Keep left if true; Keep right if false
	void crop(const float multiplier, const bool left = true)
	{
		assert(multiplier > 0.0f);
		assert(multiplier < 1.0f);

		int32 newSize = 3 * width * height;
		uint8* newData = new uint8[newSize];

		int32 newWidth = width * multiplier;
		int32 start = left ? 0 : newWidth;
		int32 end = left ? newWidth : width;

		for (int32 j = 0; j < height; j++)
		{
			for (int32 i = start; i < end; i++)
			{
				newData[3 * (i + (j * (int32)(width - newWidth))) + 0] = data[3 * (i + (j * width)) + 0];
				newData[3 * (i + (j * (int32)(width - newWidth))) + 1] = data[3 * (i + (j * width)) + 1];
				newData[3 * (i + (j * (int32)(width - newWidth))) + 2] = data[3 * (i + (j * width)) + 2];
			}
		}

		memset(data, 0, 3 * width * height);
		delete data;
		data = nullptr;

		scaleH *= 1.0f / multiplier;
		scaleW *= multiplier;
		width = newWidth;
		data = newData;
	}
};

struct Pixel32
{
	uint32 aRGB;
};

class Texture32 : public Texture
{
private:
	uint32* data = nullptr;

public:
	~Texture32()
	{
		if (data != nullptr)
		{
			delete[] data;
			data = nullptr;
		}
	}

	const int32 headerSize() const { return 66; };
	void writeData(FILE* f) override
	{
		// allocate 3 bytes per pixel
		int size = width * height;
		data = new uint32[size];

		// read the rest of the data at once
		fread(data, sizeof(uint32), size, f);
	}

	// Divide into segments peices and draw the content in index
	uint32 lookUp(const float y, const float x, const int32 segments = 1, const int32 index = 1) const override
	{
		// Flip values (array stored differently)
		int32 xIndex;
		int32 yIndex;

		yIndex = (x * (float)width) / scaleW;
		xIndex = (((y * (float)height) / (scaleH * (float)index)) * 
			((1.0f * (float)index) / (float)segments)) + (float)height * 2.0f * ((float)index - 1.0f);

		return data[xIndex + (yIndex * width)];
	}
};