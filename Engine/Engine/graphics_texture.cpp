#include "utils.h"
#include "graphics.h"
#include "graphics_texture.h"
#include <strstream>
#include <fstream>
#include <iostream>
#include <assert.h>

#define HEADER_SIZE_24_BIT_BMP (54)
#define HEADER_SIZE_32_BIT_BMP (66)

Texture::Texture(TextureType textureType, const char* filename, const int sectionWidth, const int sectionHeight)
	: textureType(textureType)
{
	if (!loadTextureFromBMP(filename, sectionWidth, sectionHeight))
	{
		std::cerr << "Error loading " << filename << "\n";
	}
}

Texture::~Texture()
{
	if (data != nullptr)
	{
		delete[] data;
		data = nullptr;
	}
}

bool Texture::loadTextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight)
{
	int i;
	FILE* file;
	errno_t err = fopen_s(&file, filename, "r");
	if (err != 0)
	{
		std::cerr << "Error opening file " << filename << "\n";
		return false;
	}

	int headerSize;
	if (textureType == TextureType::RGB)
	{
		headerSize = HEADER_SIZE_24_BIT_BMP;
	}
	else if (textureType == TextureType::RGBA)
	{
		headerSize = HEADER_SIZE_32_BIT_BMP;
	}
	else
	{
		return false;
	}
	
	uint8* info = new uint8[headerSize];

	// read header
	fread(info, sizeof(uint8), headerSize, file);

	// Check valid
	const int bitOffset = info[10];
	std::cout << "bitOffset: " << bitOffset << std::endl;

	if (bitOffset != headerSize)
	{
		std::cerr << "Error reading " << filename << "\n";
		delete[] info;
		info = nullptr;
		return false;
	}

	// extract image height and width from header
	width = *(int*)& info[18];
	height = *(int*)& info[22];

	delete[] info;
	info = nullptr;

	assert(height > 0);
	scale = (float)width / (float)height;

	assert(sectionWidth > 0);
	assert(sectionHeight > 0);
	if (width % sectionWidth != 0 || height % sectionHeight != 0)
	{
		std::cerr << "Error loading texture from "
			<< filename << ": does not allign with "
			<< "sectionWidth or sectionHeight. "
			<< "Width = " << sectionWidth
			<< "Height = " << sectionHeight
			<< "\n";
		return false;
	}

	maxCylcesX = width / sectionWidth;
	maxCylcesY = height / sectionHeight;

	stepX = 1.0f / maxCylcesX;
	stepY = 1.0f / maxCylcesY;

	std::cout
		<< "width=" << width << " height=" << height
		<< " scale=" << scale
		<< " stepX=" << stepX << " stepY=" << stepY
		<< "\n";

	int size;
	if (textureType == TextureType::RGB)
	{
		size = 3 * width * height;
		data = new uint8[size];
		fread(data, sizeof(uint8), size, file);
	}
	else if (textureType == TextureType::RGBA)
	{
		size = width * height;
		data = new uint[size];
		fread(data, sizeof(uint), size, file);
	}
	else
	{
		return false;
	}

	fclose(file);

	return true;
}

// Take normalised x and y values and maps them to texture coords
uint Texture::lookUp(float x, float y, int cycleX, int cycleY) const
{
	if (textureType == TextureType::RGB)
	{
		std::swap(x, y);  // flip

		int xInd, yInd;
		xInd = x * (float)width / scale;
		yInd = y * (float)height * scale;

		xInd += 1;

		xInd = height - xInd;  // flip

		uint8* d = (uint8*)data;

		return rgbToHex(
			d[3 * (xInd * width + yInd) + 2],
			d[3 * (xInd * width + yInd) + 1],
			d[3 * (xInd * width + yInd) + 0]);
	}
	else if (textureType == TextureType::RGBA)
	{
		clamp(0, &cycleX, maxCylcesX);
		clamp(0, &cycleY, maxCylcesY);

		// Get texture slice
		x = (x * stepX) + (stepX * cycleX);
		y = (y * stepY) + (stepY * cycleY);

		// Get texture coordinates
		int xInd, yInd;
		xInd = x * (float)width;
		yInd = y * (float)height;

		uint* d = (uint*)data;

		return d[xInd + (yInd * width)];
	}

	return 0x000000;  // error
}

//TextureRGB::~TextureRGB()
//{
//	if (data != nullptr)
//	{
//		delete[] data;
//		data = nullptr;
//	}
//}
//
//bool TextureRGB::LoadTextureFromBMP(const char* filename)
//{
//	int i;
//	FILE* f;
//	errno_t err = fopen_s(&f, filename, "r");
//	if (err != 0)
//	{
//		std::cerr << "Error opening file " << filename << "\n";
//		return false;
//	}
//
//	uint8* info = new uint8[HEADER_SIZE_24_BIT_BMP];
//
//	// read the 54-byte header
//	fread(info, sizeof(uint8), HEADER_SIZE_24_BIT_BMP, f);
//
//	const int bOffset = info[10];
//
//	std::cout << "Offset: " << bOffset << std::endl;
//
//	// extract image height and width from header
//	width = *(int*)& info[18];
//	height = *(int*)& info[22];
//
//	assert(height > 0);
//	scale = (float)width / (float)height;
//
//	std::cout << "TextureRGB width=" << width << " height=" << height
//		<< " scale=" << scale << "\n";
//
//	// allocate 3 bytes per pixel
//	int size = 3 * width * height;
//	data = new uint8[size];
//
//	// read the rest of the data at once
//	fread(data, sizeof(uint8), size, f);
//
//	fclose(f);
//
//	delete[] info;
//	info = nullptr;
//
//	return true;
//}
//
//const uint TextureRGB::readData(int x, int y) const
//{
//	x = height - x;  // flip
//
//	uint8* d = (uint8*)data;
//
//	return rgbToHex(
//		d[3 * (x * width + y) + 2],
//		d[3 * (x * width + y) + 1],
//		d[3 * (x * width + y) + 0]);
//}
//
//uint TextureRGB::lookUp(const float y, const float x, const int segments, const int index) const
//{
//	int xInd, yInd;
//	xInd = x * (float)width / scale;
//	yInd = y * (float)height * scale;
//
//	return readData(xInd + 1, yInd);
//}
//
////TextureRGBA::TextureRGBA(const char* filename, const int sectionWidth, const int sectionHeight)
////{
////	if (!loadTextureFromBMP(filename, sectionWidth, sectionHeight))
////	{
////		std::cerr << "Error loading " << filename << "\n";
////	}
////}
//
//TextureRGBA::~TextureRGBA()
//{
//	if (data != nullptr)
//	{
//		delete[] data;
//		data = nullptr;
//	}
//}
//
//bool TextureRGBA::loadTextureFromBMP(const char* filename, const int sectionWidth, const int sectionHeight)
//{
//	int i;
//	FILE* file;
//	errno_t err = fopen_s(&file, filename, "r");
//	if (err != 0)
//	{
//		std::cerr << "Error opening file " << filename << "\n";
//		return false;
//	}
//
//	uint8* info = new uint8[HEADER_SIZE_32_BIT_BMP];
//
//	// read header
//	fread(info, sizeof(uint8), HEADER_SIZE_32_BIT_BMP, file);
//
//	// Check valid
//	const int bitOffset = info[10];
//	std::cout << "bitOffset: " << bitOffset << std::endl;
//
//	if (bitOffset != HEADER_SIZE_32_BIT_BMP)
//	{
//		std::cerr << "Error reading " << filename << "\n";
//		delete[] info;
//		info = nullptr;
//		return false;
//	}
//
//	// extract image height and width from header
//	width = *(int*)& info[18];
//	height = *(int*)& info[22];
//
//	assert(height > 0);
//	scale = width / height;
//
//	assert(sectionWidth > 0);
//	assert(sectionHeight > 0);
//	if (width % sectionWidth != 0 || height % sectionHeight != 0)
//	{
//		std::cerr << "Error loading texture from " 
//			<< filename << ": does not allign with "
//			<< "sectionWidth or sectionHeight. "
//			<< "Width = " << sectionWidth 
//			<< "Height = " << sectionHeight
//			<< "\n";
//		delete[] info;
//		info = nullptr;
//		return false;
//	}
//
//	maxCylcesX = width / sectionWidth;
//	maxCylcesY = height / sectionHeight;
//
//	stepX = 1.0f / maxCylcesX;
//	stepY = 1.0f / maxCylcesY;
//
//	std::cout 
//		<< "width=" << width << " height=" << height 
//		<< " scale=" << scale 
//		<< " stepX=" << stepX << " stepY=" << stepY
//		<< "\n";
//
//	int size = width * height;
//	data = new uint[size];
//	fread(data, sizeof(uint), size, file);
//
//	fclose(file);
//
//	delete[] info;
//	info = nullptr;
//
//	return true;
//}
//
//
//// Take normalised x and y values and maps them to texture coords
//uint TextureRGBA::lookUp(float x, float y, int cycleX, int cycleY)
//{	
//	Clamp(0, &cycleX, maxCylcesX);
//	Clamp(0, &cycleY, maxCylcesY);
//
//	// Get texture slice
//	x = (x * stepX) + (stepX * cycleX);
//	y = (y * stepY) + (stepY * cycleY);
//
//	// Get texture coordinates
//	int xInd, yInd;
//	xInd = x * (float)width;
//	yInd = y * (float)height;
//
//	uint* d = (uint*)data;
//
//	return d[xInd + (yInd * width)];
//}
