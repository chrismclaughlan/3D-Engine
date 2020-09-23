#pragma once
#include "types.h"
#include "gui.h"
#include <strstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <assert.h>

//class Texture
//{
//protected:
//	int width = -1;
//	int height = -1;
//	float scaleW = 1.0f;
//	float scaleH = 1.0f;
//
//public:
//	virtual const int headerSize() const = 0;
//	virtual void writeData(FILE* f) = 0;
//	virtual uint lookUp(const float y, const float x, const int segments = 1, const int index = 1) const = 0;
//	bool LoadTextureFromBMP(const char* filename)
//	{
//		int i;
//		FILE* f;
//		errno_t err = fopen_s(&f, filename, "r");
//		if (err != 0)
//		{
//			std::cerr << "Error opening file " << filename << "\n";
//			return false;
//		}
//
//		uint8* info = new uint8[headerSize()];
//
//		// read the 54-byte header
//		fread(info, sizeof(uint8), headerSize(), f);
//
//		const int bOffset = info[10];
//
//		std::cout << "Offset: " << bOffset << std::endl;
//
//		// extract image height and width from header
//		width = *(int*)& info[18];
//		height = *(int*)& info[22];
//		if (width < height)
//		{
//			scaleH = height / width;
//			scaleW = 1.0f / scaleH;
//		}
//		else
//		{
//			scaleW = width / height;
//			scaleH = 1.0f / scaleW;
//		}
//
//		std::cout << "width=" << width << " height=" << height << " scaleW=" << scaleW << " scaleH=" << scaleH << "\n";
//
//		writeData(f);
//
//		fclose(f);
//
//		delete[] info;
//		info = nullptr;
//
//		return true;
//	}
//
//};
//
//class Texture24 : public Texture
//{
//private:
//	uint8* data = nullptr;
//
//public:
//	~Texture24()
//	{
//		if (data != nullptr)
//		{
//			delete[] data;
//			data = nullptr;
//		}
//	}
//
//	const int headerSize() const { return 54; };
//	static uint rgbToHex(const uint8 r, const uint8 g, const uint8 b)
//	{
//		uint h = 0x0;
//		h += r << 16;
//		h += g << 8;
//		h += b << 0;
//		return h;
//	}
//
//	void writeData(FILE* f) override
//	{
//		// allocate 3 bytes per pixel
//		int size = 3 * width * height;
//		data = new uint8[size];
//
//		// read the rest of the data at once
//		fread(data, sizeof(uint8), size, f);
//	}
//
//	const uint readData(int x, int y) const
//	{
//		x = height - x;  // flip
//
//		return rgbToHex(
//			data[3 * (x * width + y) + 2],
//			data[3 * (x * width + y) + 1],
//			data[3 * (x * width + y) + 0]);
//	}
//
//	uint lookUp(const float y, const float x, const int segments = 1, const int index = 1) const override
//	{
//
//		int xIndex = (x * width) / scaleW;
//		int yIndex = (y * height) / scaleH;
//
//		return readData(xIndex + 1, yIndex);
//	}
//
//	// bool: keep -> Keep left if true; Keep right if false
//	//void crop(const float multiplier, const bool left = true)
//	//{
//	//	assert(multiplier > 0.0f);
//	//	assert(multiplier < 1.0f);
//	//
//	//	int newSize = 3 * width * height;
//	//	uint8* newData = new uint8[newSize];
//	//
//	//	int newWidth = width * multiplier;
//	//	int start = left ? 0 : newWidth;
//	//	int end = left ? newWidth : width;
//	//
//	//	for (int j = 0; j < height; j++)
//	//	{
//	//		for (int i = start; i < end; i++)
//	//		{
//	//			newData[3 * (i + (j * (int)(width - newWidth))) + 0] = data[3 * (i + (j * width)) + 0];
//	//			newData[3 * (i + (j * (int)(width - newWidth))) + 1] = data[3 * (i + (j * width)) + 1];
//	//			newData[3 * (i + (j * (int)(width - newWidth))) + 2] = data[3 * (i + (j * width)) + 2];
//	//		}
//	//	}
//	//
//	//	memset(data, 0, 3 * width * height);
//	//	delete data;
//	//	data = nullptr;
//	//
//	//	scaleH *= 1.0f / multiplier;
//	//	scaleW *= multiplier;
//	//	width = newWidth;
//	//	data = newData;
//	//}
//};
